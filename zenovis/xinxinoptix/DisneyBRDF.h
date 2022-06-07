#pragma once

#include "zxxglslvec.h"
#include "TraceStuff.h"

namespace BRDFBasics{
static __inline__ __device__  float fresnel(float cosT){
    float v = clamp(1-cosT,0.0f,1.0f);
    float v2 = v *v;
    return v2 * v2 * v;
}
static __inline__ __device__ vec3 fresnelSchlick(vec3 r0, float radians)
{
    float exponential = powf( 1.0f - radians, 5.0f);
    return r0 + (vec3(1.0f) - r0) * exponential;
}
static __inline__ __device__ float fresnelSchlick(float r0, float radians)
{
    return mix(1.0f, fresnel(radians), r0);
}
static __inline__ __device__  float GTR1(float cosT,float a){
    if(a >= 1.0f) return 1/M_PIf;
    float t = (1+(a*a-1)*cosT*cosT);
    return (a*a-1.0f) / (M_PIf*logf(a*a)*t);
}
static __inline__ __device__  float GTR2(float cosT,float a){
    float t = (1+(a*a-1)*cosT*cosT);
    return (a*a) / (M_PIf*t*t);
}
static __inline__ __device__  float GGX(float cosT, float a){
    float a2 = a*a;
    float b = cosT*cosT;
    return 2.0f/ (1.0f + sqrtf(a2 + b - a2*b));
}
static __inline__ __device__  vec3 sampleOnHemisphere(unsigned int &seed, float roughness)
{
    float x = rnd(seed);
    float y = rnd(seed);

    float a = roughness*roughness;

	float phi = 2.0f * M_PIf * x;
	float cosTheta = sqrtf((1.0f - y) / (1.0f + (a*a - 1.0f) * y));
	float sinTheta = sqrtf(1.0f - cosTheta*cosTheta);


    return vec3(cos(phi) * sinTheta,  sin(phi) * sinTheta, cosTheta);
}
static __inline__ __device__ float pdfDiffuse(vec3 wi, vec3 n)
{
    return abs(dot(n, wi)/M_PIf);
}
static __inline__ __device__ float pdfMicrofacet(float NoH, float roughness)
{
    float a2 = roughness * roughness;
    a2 *= a2;
    float cos2Theta = NoH * NoH;
    float denom = cos2Theta * (a2 - 1.) + 1;
    if(denom == 0 ) return 0;
    float pdfDistrib = a2 / (M_PIf * denom * denom);
    return pdfDistrib;
}
static __inline__ __device__ float pdfClearCoat(float NoH, float ccAlpha)
{
    float Dr = GTR1(NoH, ccAlpha);
    return Dr;
}
}
namespace DisneyBRDF
{   
static __inline__ __device__ float pdf(
        vec3 baseColor,
        float metallic,
        float subsurface,
        float specular,
        float roughness,
        float specularTint,
        float anisotropic,
        float sheen,
        float sheenTint,
        float clearcoat,
        float clearcoatGloss,
        vec3 N,
        vec3 T,
        vec3 B,
        vec3 wi,
        vec3 wo)
    {
        vec3 n = N;
        float spAlpha = max(0.001f, roughness);
        float ccAlpha = mix(0.1f, 0.001f, clearcoatGloss);
        float diffRatio = 0.5f*(1.0f - metallic);
        float spRatio = 1.0f - diffRatio;

        vec3 half = normalize(wi + wo);

        float cosTheta = abs(dot(n, half));
        float pdfGTR2 = BRDFBasics::GTR2(cosTheta, spAlpha) * cosTheta;
        float pdfGTR1 = BRDFBasics::GTR1(cosTheta, ccAlpha) * cosTheta;

        float ratio = 1.0f/(1.0f + clearcoat);
        float pdfSpec = mix(pdfGTR1, pdfGTR2, ratio)/(4.0f * abs(dot(wo, half)));
        float pdfDiff = abs(dot(wi, n)) * (1.0f/M_PIf);

        return diffRatio * pdfDiff + spRatio * pdfSpec;
    }

static __inline__ __device__ vec3 sample_f(
        unsigned int &seed, 
        vec3 baseColor,
        float metallic,
        float subsurface,
        float specular,
        float roughness,
        float specularTint,
        float anisotropic,
        float sheen,
        float sheenTint,
        float clearcoat,
        float clearcoatGloss,
        vec3 N,
        vec3 T,
        vec3 B,
        vec3 wo,
        float &is_refl)
    {
        
        float ratiodiffuse = (1.0f - metallic)/2.0f;
        float p = rnd(seed);
        
        Onb tbn = Onb(N);
        
        vec3 wi;
        
        if( p < ratiodiffuse){
            //sample diffuse lobe
            
            vec3 P = BRDFBasics::sampleOnHemisphere(seed, 1.0f);
            wi = P;
            tbn.inverse_transform(wi);
            wi = normalize(wi);
            is_refl = 0;
        }else{
            //sample specular lobe.
            float a = max(0.001f, roughness);
            
            vec3 P = BRDFBasics::sampleOnHemisphere(seed, a*a);
            vec3 half = normalize(P);
            tbn.inverse_transform(half);            
            wi = half* 2.0f* dot(normalize(wo), half) - normalize(wo); //reflection vector
            wi = normalize(wi);
            is_refl = 1;
        }
        
        return wi;
    }
static __inline__ __device__ vec3 eval(
        vec3 baseColor,
        float metallic,
        float subsurface,
        float specular,
        float roughness,
        float specularTint,
        float anisotropic,
        float sheen,
        float sheenTint,
        float clearcoat,
        float clearcoatGloss,
        vec3 N,
        vec3 T,
        vec3 B,
        vec3 wi,
        vec3 wo)
    {
        vec3 wh = normalize(wi+ wo);
        float ndoth = dot(N, wh);
        float ndotwi = dot(N, wi);
        float ndotwo = dot(N, wo);
        float widoth = dot(wi, wh);

        if(ndotwi <=0 || ndotwo <=0 )
            return vec3(0,0,0);

        vec3 Cdlin = baseColor;
        float Cdlum = 0.3f*Cdlin.x + 0.6f*Cdlin.y + 0.1f*Cdlin.z;

        vec3 Ctint = Cdlum > 0.0f ? Cdlin / Cdlum : vec3(1.0f,1.0f,1.0f);
        vec3 Cspec0 = mix(specular*0.08f*mix(vec3(1,1,1), Ctint, specularTint), Cdlin, metallic);
        vec3 Csheen = mix(vec3(1.0f,1.0f,1.0f), Ctint, sheenTint);

        //diffuse
        float Fd90 = 0.5f + 2.0f * ndoth * ndoth * roughness;
        float Fi = BRDFBasics::fresnel(ndotwi);
        float Fo = BRDFBasics::fresnel(ndotwo);
        
        float Fd = (1 +(Fd90-1)*Fi)*(1+(Fd90-1)*Fo);

        float Fss90 = widoth*widoth*roughness;
        float Fss = mix(1.0f, Fss90, Fi) * mix(1.0f,Fss90, Fo);
        float ss = 1.25f * (Fss *(1.0f / (ndotwi + ndotwo) - 0.5f) + 0.5f);

        float a = max(0.001, roughness);
        float Ds = BRDFBasics::GTR2(ndoth, a);
        float Dc = BRDFBasics::GTR1(ndoth, mix(0.1f, 0.001f, clearcoatGloss));

        float roughg = sqrtf(roughness*0.5f + 0.5f);
        float Gs = BRDFBasics::GGX(ndotwo, roughness) * BRDFBasics::GGX(ndotwi, roughness);

        float Gc = BRDFBasics::GGX(ndotwo, 0.25) * BRDFBasics::GGX(ndotwi, 0.25f);

        float Fh = BRDFBasics::fresnel(widoth);
        vec3 Fs = mix(Cspec0, vec3(1.0f,1.0f,1.0f), Fh);
        float Fc = mix(0.04f, 1.0f, Fh);

        vec3 Fsheen = Fh * sheen * Csheen;

        return ((1/M_PIf) * mix(Fd, ss, subsurface) * Cdlin + Fsheen) * (1.0f - metallic)
        + Gs*Fs*Ds + 0.25f*clearcoat*Gc*Fc*Dc;
    }
}




