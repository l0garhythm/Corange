uniform mat4 world_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

uniform float time;

varying vec3 normal;
varying vec3 position;

vec3 spike_wave( vec2 direction, vec2 position, float wavelength, float spikiness ) {
	
  direction = normalize(direction);
	
  vec3 result;
  result.y = sin( dot(direction, position / wavelength) ) + 1.0;	
  result.xz = 0.5 * spikiness * direction * cos( dot(direction, position / wavelength) );	

  return result;
}


void main()
{
	
  vec3 difference = vec3(0,0,0);

  float time_v = time * 3.5;
  
  vec3 vposition = gl_Vertex.xyz * 2;
  
  difference += 0.75 * spike_wave( vec2(1,0) , vposition.xz * 0.912 + time_v * 1.211 , 2.3, 1.0 );
  difference += 0.5 * spike_wave( vec2(1.0,0.62) , vposition.xz * 0.932 + time_v * 0.971 , 1.85, 1.0 );
  difference += 0.33 * spike_wave( vec2(0.41,1.0) , vposition.xz * 1.112 + time_v * 1.314 , 2.21, 1.0 );
  difference += 0.75 * spike_wave( vec2(0.1,0.51) , vposition.xz * 1.321 + time_v * 0.823 , 3.12, 1.0 );
  
  //difference.y = sin( gl_Vertex.x + time );
  
  vec4 position_temp = gl_Vertex;
  vec3 normal_temp = gl_Normal;
  
  position_temp.xyz += difference;
  normal_temp.xyz += 0.25 * vec3( -difference.x, difference.y, -difference.z);
  
  normal = normal_temp;
  position = gl_Vertex.xyz / position_temp.w;
  gl_Position = proj_matrix * view_matrix * world_matrix * position_temp;
}
