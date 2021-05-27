Matrix Light::CalculateCropMatrix(Frustum splitFrustum) 
{   
    Matrix lightViewProjMatrix = viewMatrix * projMatrix;   

    // Find boundaries in light's clip space   
    BoundingBox cropBB = CreateAABB(splitFrustum.AABB, lightViewProjMatrix);   

    // Use default near-plane value   
    cropBB.min.z = 0.0f;   

    // Create the crop matrix   
    float scaleX, scaleY, scaleZ;   
    float offsetX, offsetY, offsetZ;   
    scaleX = 2.0f / (cropBB.max.x - cropBB.min.x);   
    scaleY = 2.0f / (cropBB.max.y - cropBB.min.y);   
    offsetX = -0.5f * (cropBB.max.x + cropBB.min.x) * scaleX;   
    offsetY = -0.5f * (cropBB.max.y + cropBB.min.y) * scaleY;   
    scaleZ = 1.0f / (cropBB.max.z - cropBB.min.z);   
    offsetZ = -cropBB.min.z * scaleZ;   

    return Matrix( scaleX,     0.0f,     0.0f,  0.0f,                    
                0.0f,   scaleY,     0.0f,  0.0f,                    
                0.0f,     0.0f,   scaleZ,  0.0f,                 
                offsetX,  offsetY,  offsetZ,  1.0f); 
}


 Matrix Light::CalculateCropMatrix(ObjectList casters, ObjectList receivers, Frustum frustum) 
 {   
    // Bounding boxes   
    BoundingBox receiverBB, casterBB, splitBB;   
    Matrix lightViewProjMatrix = viewMatrix * projMatrix;   

    // Merge all bounding boxes of casters into a bigger "casterBB".   
    for(int i = 0; i < casters.size(); i++)
    {     
        BoundingBox bb = CreateAABB(casters[i]->AABB, lightViewProjMatrix);     
        casterBB.Union(bb);   
    }   

    // Merge all bounding boxes of receivers into a bigger "receiverBB".   
    for(int i = 0; i < receivers.size(); i++)
    {       
        bb = CreateAABB(receivers[i]->AABB,                       lightViewProjMatrix);     
        receiverBB.Union(bb);   
    }   

    // Find the bounding box of the current split    
    // in the light's clip space.   
    splitBB = CreateAABB(splitFrustum.AABB, lightViewProjMatrix);   

    // Scene-dependent bounding volume   
    BoundingBox cropBB;   
    cropBB.min.x = Max(Max(casterBB.min.x, receiverBB.min.x), splitBB.min.x);   
    cropBB.max.x = Min(Min(casterBB.max.x, receiverBB.max.x), splitBB.max.x);   
    cropBB.min.y = Max(Max(casterBB.min.y, receiverBB.min.y), splitBB.min.y);   
    cropBB.max.y = Min(Min(casterBB.max.y, receiverBB.max.y), splitBB.max.y);   
    cropBB.min.z = Min(casterBB.min.z, splitBB.min.z);   
    cropBB.max.z = Min(receiverBB.max.z, splitBB.max.z);   

    // Create the crop matrix.   
    float scaleX, scaleY, scaleZ;   
    float offsetX, offsetY, offsetZ;   

    scaleX = 2.0f / (cropBB.max.x - cropBB.min.x);   
    scaleY = 2.0f / (cropBB.max.y - cropBB.min.y);   
    offsetX = -0.5f * (cropBB.max.x + cropBB.min.x) * scaleX;   
    offsetY = -0.5f * (cropBB.max.y + cropBB.min.y) * scaleY;   
    scaleZ = 1.0f / (cropBB.max.z – cropBB.min.z);   
    offsetZ = -cropBB.min.z * scaleZ;   

    return Matrix(scaleX, 0.0f, 0.0f, 0.0f,                 
                0.0f, scaleY, 0.0f, 0.0f,                 
                0.0f, 0.0f, scaleZ, 0.0f,                 
                offsetX, offsetY, offsetZ, 1.0f); 
}



 for(int i = 0; i < numSplits; i++) 
 {   
    // Compute frustum of current split part.   
    splitFrustum = camera->CalculateFrustum(splitPos[i], splitPos[i+1]);   
    casters = light->FindCasters(splitFrustum);   
    // Compute light's transformation matrix for current split part.   
    cropMatrix = light->CalculateCropMatrix(receivers, casters, splitFrustum);   
    splitViewProjMatrix = light->viewMatrix * light->projMatrix * cropMatrix;   
    // Texture matrix for current split part   
    textureMatrix = splitViewProjMatrix * texScaleBiasMatrix;   
    // Render current shadow map.   
    ActivateShadowMap();   
    RenderObjects(casters, splitViewProjMatrix);   
    DeactivateShadowMap();   
    // Render shadows for current split part.   
    SetDepthRange(splitPos[i], splitPos[i+1]);   
    SetShaderParam(textureMatrix);   
    RenderObjects(receivers, camera->viewMatrix * camera->projMatrix); 
} 



sampler2D shadowMapSampler[numSplits]; 
void VS_RenderShadows(   in float4 pos : POSITION, 
        // Object-space coordinates    
        out float4 posOut : POSITION, 
        // Clip-space coordinates    
        out float4 texCoord[numSplits+1] : TEXCOORD) 
        // Texture coordinates 
{   
    // Calculate world position.    
    float4 posWorld = mul(pos, worldMatrix);   
    // Transform vertex.   
    posOut = mul(posWorld, viewProjMatrix);   
    // Store view-space position in the first texture    
    // coordinate register.   
    texCoord[0] = mul(posWorld, viewMatrix)   
    // Store shadow-map coordinates in the remaining    
    // texture coordinate registers.    
    for(int i = 0; i < numSplits; i++)   
    {     
        texCoord[i+1] = mul(posWorld, textureMatrix[i]);   
    } 
} 

float4 PS_RenderShadows(   float4 texCoord[numSplits+1] : TEXCOORD): COLOR 
{   
    float light = 1.0f;   
    // Fetch view-space distance from first texture coordinate register.    
    float distance = texCoord[0].z;   
    for(int i = 0; i < numSplits; i++)   
    {     
        if(distance < splitPlane[i])     
        {       
            float depth = texCoord[i+1].z/ texCoord[i+1].w;       
            float depthSM = tex2Dproj(shadowMapSampler[i], texCoord[i+1]);       
            // Standard depth comparison       
            light = (depth < depthSM) ? 1.0f : 0.0f;       
            break;     
        }   
    
    }   
    return light; 
} 
