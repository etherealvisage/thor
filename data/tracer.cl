typedef struct {
    float s;
    float3 v;
} quaternion;

quaternion quat_conj(quaternion q) {
    q.s = -q.s;
    return q;
}

quaternion quat_mul(quaternion q1, quaternion q2) {
    quaternion q;
    q.s = q1.s*q2.s - dot(q1.v, q2.v);
    q.v = q1.s * q2.v + q2.s*q1.v + cross(q1.v, q2.v);
    return q1;
}

float3 quat_rot(quaternion quat, float3 vec) {
    quaternion q;
    q.s = 0;
    q.v = vec;
    return quat_mul(quat_mul(quat, q), quat_conj(quat)).v;
}

float checkIntersection(float3 rayPos, float3 rayDir, __global float *sphere) {
    float3 spherePos = (float3)(sphere[0], sphere[1], sphere[2]);

    float3 modPos = rayPos - spherePos;

    float d = -(dot(rayDir, modPos));

    float x = dot(rayDir, modPos);
    x *= x;
    x -= dot(modPos, modPos);
    x += sphere[3]*sphere[3];
    if(x < 0) return -1.0;

    // find closer intersection of the two...
    double v = d - sqrt(x);
    if(v < 0) v = d + sqrt(x);
    if(v < 0) return -1.0;
    return v;
}

/*float findIntersection(float *spheres, int sphereCount, float3 rayPos,
    float3 rayDir) {
    
}*/

void __kernel tracer(__write_only image2d_t image, __global int *counter,
    __read_only __global float *camera,
    __read_only __global float *spheres, int sphereCount) {

    int width = get_image_width(image);
    int height = get_image_height(image);
    int maxPixel = width * height;
    float aratio = (float)width/(float)height;

    /* Unpack camera config */
    float3 cameraPos = (float3)(camera[0], camera[1], camera[2]);
    quaternion cameraRot;
    cameraRot.s = camera[4];
    cameraRot.v = (float3)(camera[5], camera[6], camera[7]);

    /* Camera calcs */
    /*float3 cameraForward = quat_rot(cameraRot, (float3)(0,0,1));
    float3 cameraPlaneX = quat_rot(cameraRot, (float3)(aratio/width,0,0));
    float3 cameraPlaneY = quat_rot(cameraRot, (float3)(0,1/(float)height,0));*/
    float3 cameraForward = (float3)(0,0,1);
    float3 cameraPlaneX = (float3)(1/(float)width, 0, 0);
    float3 cameraPlaneY = (float3)(0,1/(float)width, 0);
    float dist = atan(camera[3]/180 * M_PI / 2);

    float3 cameraPlaneOrigin = cameraPos + cameraForward * dist;

    while(1) {
        int pixelID = atomic_inc(counter);
        if(pixelID >= maxPixel) break;

        int pixelX = pixelID % width;
        int pixelY = pixelID / width;

        int cameraX = pixelX - (width/2);
        int cameraY = pixelY - (height/2);

        float3 rayDir = 
            (cameraPlaneOrigin
                + (cameraPlaneX * cameraX)
                + (cameraPlaneY * cameraY)) - cameraPos;
        //float3 rayDir = (float3)(0,0,1);

        rayDir /= length(rayDir);

        if(checkIntersection(cameraPos, rayDir, spheres) == -1.0) {
            // no intersection
            write_imagef(image, (int2)(pixelX, pixelY), (float4)(0,0,0,1));
        }
        else {
            // intersection
            write_imagef(image, (int2)(pixelX, pixelY), (float4)(1,0,0,1));
        }

        /*write_imagef(image, (int2)(pixelX, pixelY),
            (float4)((float)pixelX/(float)width,(float)pixelY/(float)height,1,1));*/
    }
}
