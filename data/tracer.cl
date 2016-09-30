#define SPHERE_STRIDE 8
#define LIGHT_STRIDE 12
typedef struct globalData {
    float spheres[4096];
    int sphereCount;
    float lights[96];
    int lightCount;
} globalData;

float4 quat_conj(float4 q) {
    q.xyz = -q.xyz;
    return q;
}

float4 quat_mul(float4 q1, float4 q2) {
    float4 result;
    result.xyz = q1.w*q2.xyz + q2.w*q1.xyz + cross(q1.xyz, q2.xyz);
    result.w = q1.w * q2.w - dot(q1.xyz, q2.xyz);
    return result;
}

float3 quat_rot(float4 q, float3 v) {
    float4 t = (float4)(v.xyz, 0);
    return quat_mul(quat_mul(q, t), quat_conj(q)).xyz;
}

float checkIntersection(float3 rayPos, float3 rayDir, __private float *sphere) {
    float3 spherePos = (float3)(sphere[0], sphere[1], sphere[2]);

    float3 modPos = rayPos - spherePos;

    float d = -(dot(rayDir, modPos));

    float x = dot(rayDir, modPos);
    x *= x;
    x -= dot(modPos, modPos);
    x += sphere[3]*sphere[3];
    if(x < 0) return -1.0;

    // find closer intersection of the two...
    float v = d - sqrt(x);
    if(v < 0) v = d + sqrt(x);
    if(v < 0) return -1.0;
    return v;
}

float3 calculateNormal(__private float *sphere, float3 rayPos) {
    float3 x = (float3)(sphere[0], sphere[1], sphere[2]);

    x = rayPos - x;
    x /= length(x);

    return x;
}

float findClosestIntersection(float3 rayPos, float3 rayDir,
    __private float *spheres, int sphereCount, int *sphereIndex) {

    float closest = 1e9;
    int bestID = -1;

    for(int i = 0; i < sphereCount; i ++) {
        float in = checkIntersection(rayPos, rayDir,
            spheres + i*SPHERE_STRIDE);
        if(in == -1) continue;
        if(in < closest) closest = in, bestID = i;
    }

    *sphereIndex = bestID;
    return closest == 1e9 ? -1.0 : closest;
}

void calculateShading(__private globalData *gd, float3 from, float3 rayDir,
    float3 normal, int sphereID, float4 *colour, float *colourWeight) {

    float ref = gd->spheres[sphereID*SPHERE_STRIDE + 7];
    float4 sphereColour = (float4)(
        gd->spheres[sphereID*SPHERE_STRIDE + 4],
        gd->spheres[sphereID*SPHERE_STRIDE + 5],
        gd->spheres[sphereID*SPHERE_STRIDE + 6],
        1.0);

    for(int l = 0; l < gd->lightCount; l ++) {
        float3 lightPos = (float3)(
            gd->lights[l*LIGHT_STRIDE + 0],
            gd->lights[l*LIGHT_STRIDE + 1],
            gd->lights[l*LIGHT_STRIDE + 2]
            );
        float3 toLight = (from - lightPos);
        float toLightDist = length(toLight);
        toLight /= toLightDist;
        int index;
        float dist = findClosestIntersection(from, toLight, gd->spheres,
            gd->sphereCount, &index);
        float mul = 0.0;
        if(index == -1 || dist <= toLightDist) {
            mul = 1.0;
        }

        float3 lightAmb = (float3)(
            gd->lights[l*LIGHT_STRIDE + 3],
            gd->lights[l*LIGHT_STRIDE + 4],
            gd->lights[l*LIGHT_STRIDE + 5]);
        float3 lightDiff = (float3)(
            gd->lights[l*LIGHT_STRIDE + 6],
            gd->lights[l*LIGHT_STRIDE + 7],
            gd->lights[l*LIGHT_STRIDE + 8]);

        float3 lightCoeff =
            (lightAmb + lightDiff*mul*fmax(0.f,-dot(normal, toLight)));

        *colour += *colourWeight * (1-ref) * sphereColour * (float4)(lightCoeff, 1.0);
    }
    *colourWeight *= ref;
}

float3 reflect(float3 vector, float3 normal) {
    return vector - 2*(dot(vector, normal)) * normal;
}

void __kernel tracer(__write_only image2d_t image, __global int *counter,
    __read_only __global float *camera,
    __read_only __global float *spheres, int sphereCount,
    __read_only __global float *lights, int lightCount) {

    __private globalData gd;
    for(int i = 0; i < sphereCount * SPHERE_STRIDE; i ++) {
        gd.spheres[i] = spheres[i];
    }
    gd.sphereCount = sphereCount;
    for(int i = 0; i < lightCount * LIGHT_STRIDE; i ++) {
        gd.lights[i] = lights[i];
    }
    gd.lightCount = lightCount;

    int width = get_image_width(image);
    int height = get_image_height(image);
    int maxPixel = width * height;
    float aratio = (float)width/(float)height;

    /* Unpack camera config */
    float3 cameraPos = (float3)(camera[0], camera[1], camera[2]);
    float4 cameraRot = (float4)(camera[5], camera[6], camera[7], camera[4]);

    /* Camera calcs */
    float3 cameraForward = quat_rot(cameraRot, (float3)(0,0,1));
    float3 cameraPlaneX = quat_rot(cameraRot, (float3)(aratio/(float)width,0,0));
    float3 cameraPlaneY = quat_rot(cameraRot, (float3)(0,1/(float)height,0));
    float dist = atan(camera[3]/180 * M_PI / 2);

    float3 cameraPlaneOrigin = cameraPos + cameraForward * dist;

    int depth = -1;

    float3 rayDir, rayPos;
    float colourWeight;
    float4 colour;

    int pixelX, pixelY;

    while(1) {
        if(depth == -1) {
            int pixelID = atomic_inc(counter);
            if(pixelID >= maxPixel) break;

            pixelX = pixelID % width;
            pixelY = pixelID / width;

            int cameraX = pixelX - (width/2);
            int cameraY = pixelY - (height/2);

            rayDir = 
                (cameraPlaneOrigin
                    + (cameraPlaneX * cameraX)
                    + (cameraPlaneY * cameraY)) - cameraPos;

            rayPos = cameraPos;
            rayDir /= length(rayDir);

            colourWeight = 1.0;
            colour = (float4)(0,0,0,0);

            depth = 4;
        }
        else if(depth == 0) {
            // done for now
            depth = -1;
            write_imagef(image, (int2)(pixelX, pixelY), colour);
        }
        else {
            int sphereID;
            float isect = findClosestIntersection(rayPos, rayDir, gd.spheres,
                sphereCount, &sphereID);

            if(sphereID != -1) {

                rayPos = rayPos + rayDir * isect;
                float3 normal = calculateNormal(
                    gd.spheres + (sphereID)*SPHERE_STRIDE, rayPos);
                calculateShading(&gd, rayPos, rayDir, normal, sphereID,
                    &colour, &colourWeight);

                rayDir = reflect(rayDir, normal);

                rayPos += rayDir * 0.01f;

                depth --;
            }
            else {
                depth = 0;
            }
        }
    }
}
