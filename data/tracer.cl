/*
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
    float4 t = (v.x, v.y, v.z, 0);
    return quat_mul(quat_mul(q, t), quat_conj(q)).xyz;
}
*/

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
    float v = d - sqrt(x);
    if(v < 0) v = d + sqrt(x);
    if(v < 0) return -1.0;
    return v;
}

float3 calculateNormal(__global float *sphere, float3 rayPos) {
    float3 x = (float3)(sphere[0], sphere[1], sphere[2]);

    x = rayPos - x;
    x /= length(x);

    return x;
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
    float4 cameraRot = (float4)(camera[5], camera[6], camera[7], camera[4]);
    //float4 cameraRot = (float4)(0,0,0,1);

    /* Camera calcs */
    float3 cameraForward = quat_rot(cameraRot, (float3)(0,0,1));
    float3 cameraPlaneX = quat_rot(cameraRot, (float3)(aratio/(float)width,0,0));
    float3 cameraPlaneY = quat_rot(cameraRot, (float3)(0,1/(float)height,0));
    /*float3 cameraForward = (float3)(0,0,1);*/
    //cameraForward = (float3)(0,0,1);
    //cameraForward.z = .01;
    /*float3 cameraPlaneX = (float3)(1/(float)width, 0, 0);
    float3 cameraPlaneY = (float3)(0,1/(float)width, 0);*/
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
            float closest = 1e9;
            int bestID = -1;

            for(int i = 0; i < sphereCount; i ++) {
                float in = checkIntersection(rayPos, rayDir, spheres + i*4);
                if(in == -1) continue;
                //closest = min(closest, in);
                if(in < closest) closest = in, bestID = i;
            }

            if(closest != 1e9) {
                float ref = 0.5;
                colour += colourWeight * (float4)(ref,ref,ref,1.0);
                colourWeight *= ref;

                rayPos = rayPos + rayDir * closest;
                rayDir = calculateNormal(spheres + (bestID)*4, rayPos);
                rayPos += rayDir * 0.01f;
                depth --;
            }
            else {
                depth = 0;
            }
        }

        /*if(checkIntersection(cameraPos, rayDir, spheres) == -1.0) {
            // no intersection
            write_imagef(image, (int2)(pixelX, pixelY), (float4)(0,0,0,1));
        }
        else {
            // intersection
            write_imagef(image, (int2)(pixelX, pixelY), (float4)(0.1,0.1,0.1,1));
        }*/
    }
}
