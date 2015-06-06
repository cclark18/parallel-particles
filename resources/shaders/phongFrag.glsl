#define MAX_LIGHTS 10
uniform mat4 uModelMatrix;
uniform vec3 UaColor;	// ambient
uniform vec3 UdColor;	// diffuse
uniform vec3 UsColor;	// specular
uniform float Ushine;
uniform vec3 uCamPos;
uniform int numLights;
uniform vec3 uLightPos;

// Cook Stuff
uniform float uMatRoughness;
uniform float uFresReflectance;

varying vec3 vNormal;
varying vec3 vCol;
varying vec3 vPos;

float cookTorrance(vec3 _normal, vec3 _light, vec3 _view, float _fresnel, float _roughness) {
  vec3  half_vec = normalize( _view + _light ); // vector H 
  // Now compute the various scalar products 
  float NdotL = max( dot(_normal, _light), 0.0 );
  float NdotV = max( dot(_normal, _view), 0.0 );
  float NdotH = max( dot(_normal, half_vec), 1.0e-7 );
  float VdotH = max( dot(_view, half_vec), 1.0e-7 );

  // geometric component
  float geometric = 2.0 * NdotH / VdotH;
  geometric = min( 1.0, geometric * min(NdotV, NdotL) );

  // roughness
  float r_sq = _roughness * _roughness;
  float NdotH_sq = NdotH * NdotH;
  float NdotH_sq_r = 1.0 / (NdotH_sq * r_sq);
  float roughness_exp = (NdotH_sq - 1.0) * ( NdotH_sq_r );
  float roughness = 0.25 * exp(roughness_exp) * NdotH_sq_r / NdotH_sq;

  // final result
  return min(1.0, _fresnel * geometric * roughness / (NdotV + 1.0e-7));
}

// CHECKPOINT!!!!!!!!!
void main() {
     
     vec3 color = vec3(0.0, 0.0, 0.0);
     vec4 aColor = vec4(0.0, 0.0, 0.0, 1.0);
     vec3 light_color = vec3(0.5, 0.5, 0.243);
     //vec3 ambient = UaColor * 0.5;
     vec3 ambient = vec3(0.05, 0.05, 0.05);
     //float bias = 0.005;
     //float visibility = 1.5;
	 float clrBleedVal = detectionLevel;
     vec3 surfacePos = vPos;
     vec3 surfaceToCamera = normalize(uCamPos - surfacePos);
     float alpha = 1.0;

   for (int i = 0; i < numLights; i++) {
     float visibility = 1.0;
     float att = 1.0;
     vec3 lightPos = allLights[i];
     vec3 surfaceToLight = normalize(lightPos - surfacePos);     
     float dist = distance(vec2(lightPos.x, lightPos.z), vec2(surfacePos.x, surfacePos.z));     
     float bias = 0.005 * tan(acos(dot(vNormal, surfaceToLight)));
     bias = clamp(bias, 0.0, 0.01);
     vec3 spotDir = normalize(coneDirection);
     float lightToSurfaceAngle = degrees(acos(dot(-surfaceToLight, spotDir)));
     
     if (lightToSurfaceAngle > coneAngle) {
     	att = 0.2;
     }

        if (texture2D(shadowMap, (ShadowCoord.xy + vec2( -0.94201624, -0.39906216) / 700.0)/ShadowCoord.w).z < (ShadowCoord.z-bias) / ShadowCoord.w) {
            visibility -= 0.1;
        }
        
        if (texture2D(shadowMap, (ShadowCoord.xy + vec2( -0.94201624, -0.39906216) / 700.0)/ShadowCoord.w).z <  (ShadowCoord.z-bias) / ShadowCoord.w) {
            visibility -= 0.1;
        }
        
        if (texture2D(shadowMap, (ShadowCoord.xy + vec2( -0.94201624, -0.39906216) / 700.0)/ShadowCoord.w).z <  (ShadowCoord.z-bias) / ShadowCoord.w) {
            visibility -= 0.1;
        }
        
        if (texture2D(shadowMap, (ShadowCoord.xy + vec2( -0.94201624, -0.39906216) / 700.0)/ShadowCoord.w).z <  (ShadowCoord.z-bias) / ShadowCoord.w) {
            visibility -= 0.1;
        }
	
	if (hasTex == 1) {
	   vec4 aDiffuse = texture2D(texture, texCoordOut);
	   float avgDiffuse = (aDiffuse.r + aDiffuse.b + aDiffuse.g)/3.0;
	   vec3 diffuse = vec3((aDiffuse.r + ((avgDiffuse - aDiffuse.r) * clrBleedVal)), 
				      (aDiffuse.g + ((avgDiffuse - aDiffuse.g) * clrBleedVal)), 
					  (aDiffuse.b + ((avgDiffuse - aDiffuse.b) * clrBleedVal)));
	   if(lightToSurfaceAngle > coneAngle) {
		 color += (1.0 /(0.5 + 0.3 * dist)) * visibility * att * diffuse;
		 //aColor += (1.0 /(0.5 + 0.3 * dist)) * visibility * att * aDiffuse; 
	   }
	   else {
		 color += (1.0 /(0.5 + 0.3 * dist)) * visibility * light_color * diffuse;
		 //aColor += (1.0 /(0.5 + 0.3 * dist)) * visibility * att * aDiffuse;
	   }

	   aColor = aDiffuse;
	}
	else {	

	   vec3 diffuse = UdColor * dot(vNormal, surfaceToLight);
	   diffuse.x = diffuse.x < 0.0 ? 0.0: diffuse.x;
	   diffuse.y = diffuse.y < 0.0 ? 0.0: diffuse.y;
	   diffuse.z = diffuse.z < 0.0 ? 0.0: diffuse.z;
	   float temp = dot(surfaceToCamera, reflect(-surfaceToLight, vNormal));
	   temp = temp < 0.0 ? 0.0: temp;
	   vec3 specular = UsColor * pow(temp, Ushine); // n=1
		float avgDiffuse = (diffuse.r + diffuse.b + diffuse.g)/3.0;
		float avgSpecular = (specular.r + specular.b + specular.g)/3.0;
		diffuse = vec3((diffuse.r + ((avgDiffuse - diffuse.r) * clrBleedVal)), 
						(diffuse.g + ((avgDiffuse - diffuse.g) * clrBleedVal)), 
						(diffuse.b + ((avgDiffuse - diffuse.b) * clrBleedVal)));

		specular = vec3((specular.r + ((avgSpecular - specular.r) * clrBleedVal)), 
						(specular.g + ((avgSpecular - specular.g) * clrBleedVal)), 
						(specular.b + ((avgSpecular - specular.b) * clrBleedVal)));
	   color += (att * (diffuse + specular));
	   alpha = 0.4;
	}
   }	
     	 
//vec2( -0.94201624, -0.39906216) / 700.0
//vec2( 0.94558609, -0.76890725)  / 700.0
//vec2( -0.094184101, -0.9293887)  / 700.0
//vec2( 0.34495938, 0.29387760) / 700.0
	/*if (texture2D(shadowMap, ShadowCoord.xy/ShadowCoord.w).z < (ShadowCoord.z-bias) / ShadowCoord.w) {
	   visibility -= 0.2;
	}
	
	if (texture2D(shadowMap, ShadowCoord.xy/ShadowCoord.w).z <  (ShadowCoord.z-bias) / ShadowCoord.w) {
           visibility -= 0.2;
        }

	if (texture2D(shadowMap, ShadowCoord.xy/ShadowCoord.w).z <  (ShadowCoord.z-bias) / ShadowCoord.w) {
           visibility -= 0.2;
	}

	if (texture2D(shadowMap, ShadowCoord.xy/ShadowCoord.w).z <  (ShadowCoord.z-bias) / ShadowCoord.w) {
           visibility = 0.4;
    	}*/

		float avgAmbient = (ambient.r + ambient.b + ambient.g)/ 3.0;
			   ambient = vec3((ambient.r + ((avgAmbient - ambient.r) * clrBleedVal)), 
						   (ambient.g + ((avgAmbient - ambient.g) * clrBleedVal)), 
						   (ambient.b + ((avgAmbient - ambient.b) * clrBleedVal)));

	//float rs = cookTorrance(vNormal, vLight, viewDir, uFresReflectance, uMatRoughness);

	/*if (hasText != 1) {
		gl_FragColor = vec4((att * ((max(dot(vNormal, vLight), 0) * (specular * rs)) + diffuse)) + ambient, 1.0);
	}
	else {
	gl_FragColor = vec4(color + avgAmbient, 1.0);
	}*/
    if (isGlass == 1) {
      gl_FragColor = aColor;
    }
    else {
      gl_FragColor = vec4(color + avgAmbient, alpha);
    }
}
