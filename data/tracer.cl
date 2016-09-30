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

void __kernel tracer(__write_only image2d_t image, __global int *counter,
    __read_only __global float *camera,
    __read_only __global float *spheres, int sphereCount) {

    for(int x = 0; x < get_image_width(image); x ++) {
        for(int y = 0; y < get_image_height(image); y ++) {
            write_imagef(image, (int2)(x, y), (float4)(.3,.3,1.0,.5));
        }
    }
}

#if 0
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
    float3 cameraForward = quat_rot(cameraRot, (float3)(0,0,1));
    float3 cameraPlaneX = quat_rot(cameraRot, (float3)(aratio/width,0,0));
    float3 cameraPlaneY = quat_rot(cameraRot, (float3)(0,1/(float)height,0));
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

        rayDir /= length(rayDir);

        write_imagef(image, (pixelX, pixelY),
            (float4)((float)pixelX/(float)width,(float)pixelY/(float)height,1,1));
    }
}
#endif
