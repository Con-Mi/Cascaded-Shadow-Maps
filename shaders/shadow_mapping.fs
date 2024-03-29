#version 420 core
out vec4 FragColor;

const int NUM_CASCADES = 6;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace[NUM_CASCADES];
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap[NUM_CASCADES];

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float cascadesSplit[6];


float ShadowCalculation( vec4 fragPosLightSpace[NUM_CASCADES] )
{
    //float depth = viewPos.z;
    float depth = gl_FragCoord.z;
    vec3 projCoords = vec3(0.0f, 0.0f, 0.0f);
    float closestDepth = 0.0f;

    for ( int i=0; i < NUM_CASCADES; ++i )
    {
        if ( depth < cascadesSplit[i] )
        {
            // perform perspective divide
            projCoords = fragPosLightSpace[i].xyz / fragPosLightSpace[i].w;
            // transform to [0,1] range
            projCoords = projCoords * 0.5 + 0.5;
            // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
            closestDepth = texture(shadowMap[i], projCoords.xy).r;
        }
        else
        {
                // perform perspective divide
            projCoords = fragPosLightSpace[0].xyz / fragPosLightSpace[0].w;
                // transform to [0,1] range
            projCoords = projCoords * 0.5 + 0.5;
                // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
            closestDepth = texture(shadowMap[0], projCoords.xy).r;
        }
    }
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main()
{
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
}