#version 420 core
in vec2 fragCoord;
out vec4 fragColor;
uniform float iTime;
uniform vec2 iResolution;
uniform vec3 cam_from;
uniform vec3 cam_at;

struct camera {
    vec3 lookfrom;
    vec3 lookat;
    vec3 vup;
    float vfov;
};

struct ray {
    vec3 origin;
    vec3 direction;
};


// transformations
mat4 translate3D(float tx, float ty, float tz) {
    return mat4(
            vec4(1., 0., 0., 0.),
            vec4(0., 1., 0., 0.),
            vec4(0., 0., 1., 0.),
            vec4(tx, ty, tz, 1.)
    );
}

mat4 rotation3D(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(
        oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
        oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
        oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
        0.0,                                0.0,                                0.0,                                1.0
    );
}

vec3 translate_point(vec3 p, float tx, float ty, float tz) {
    return (inverse(translate3D(tx, ty, tz)) * vec4(p, 1.)).xyz;
}

vec3 rotate_point(vec3 p, vec3 axis, float angle) {
    return (inverse(rotation3D(axis, angle)) * vec4(p, 1.)).xyz;
}

// SDFs and SDF manipulations
float smin( float a, float b, float k )
{
    float res = exp2( -k*a ) + exp2( -k*b );
    return -log2( res )/k;
}

float sdSphere( vec3 p, float s )
{
    return length(p)-s;
}

float sdBox( vec3 p, vec3 b )
{
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float eggSDF(vec3 p) {
    return smin(sdSphere(p, 0.6), sdSphere(translate_point(p, 0, .5, 0.), 0.3), 8.);
}

float infinite_SDF(vec3 p) {
    vec3 c = vec3(1.1, 1.1, 1.1);
    vec3 np = mod(p + 0.5 * c, c) - 0.5 * c;
    return sdSphere(np, 0.25);
}

float infiniteEggsSDF(vec3 p) {
    vec3 c = vec3(2., 2., 2.);
    vec3 np = mod(p + 0.5 * c, c) - 0.5 * c;
    return eggSDF(np);
}

float mandelbulbSDF(vec3 p) {
    vec3 z = p;
    float dr = 1.0;
    float r = 0.0;
    float Power = 2.;
    for (int i = 0; i < 10 ; i++) {
        r = length(z);
        if (r>4.0) break;

        // convert to polar coordinates
        float theta = acos(z.z/r);
        float phi = atan(z.y,z.x);
        dr =  pow( r, Power-1.0)*Power*dr + 1.0;

        // scale and rotate the point
        float zr = pow( r,Power);
        theta = theta*Power;
        phi = phi*Power;

        // convert back to cartesian coordinates
        z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
        z+=p;
    }
    return 0.5*log(r)*r/dr;
}

float spongeSDF(vec3 p) {
    float d = sdBox(p,vec3(10.0));

    float s = 1.0;
    for( int m=0; m<10; m++ )
    {
        vec3 a = mod( p*s, 2.0 )-1.0;
        s *= 3.0;
        vec3 r = abs(1.0 - 3.0*abs(a));

        float da = max(r.x,r.y);
        float db = max(r.y,r.z);
        float dc = max(r.z,r.x);
        float c = (min(da,min(db,dc))-1.0)/s;

        d = max(d,c);
    }

    return d;
}

float SDF(vec3 p) {
    return mandelbulbSDF(p);
}

// raymarching
vec3 get_ray_direction(camera c, float aspect_ratio, vec2 pos) {
    float theta = radians(c.vfov);
    float h = tan(theta / 2.);
    float viewport_height = 2.0 * h;
    float viewport_width = aspect_ratio * viewport_height;

    float focal_length = 1.0;
    
    vec3 w = normalize(c.lookfrom - c.lookat);
    vec3 u = normalize(cross(c.vup, w));
    vec3 v = cross(w, u);
    
    vec3 origin = c.lookfrom;
    vec3 horizontal = viewport_width * u;
    vec3 vertical = viewport_height * v;
    vec3 lower_left_corner = origin - horizontal / 2. - vertical / 2. - w;

    return normalize(lower_left_corner + pos.x * horizontal + pos.y * vertical - origin);
}
vec2 ray_march(ray r, int max_marching_steps, float start, float end, float epsilon) {
    float depth = start;
    float latest_i = 0.;
    for (int i = 0; i < max_marching_steps; i++) {
        float dist = SDF(r.origin + depth * r.direction);
        if (dist < epsilon) {
            return vec2(depth, latest_i);
        }
        depth += dist;
        latest_i += 1.;
        if (depth >= end) break;
    }
    return vec2(end, latest_i);
}
float shadow_ray_march(ray r, int max_marching_steps, float start, float end, float epsilon, float k, float bias) {
    float res = 1.0;
//    float bias = 0.4;
    float depth = start;
    for (int i = 0; i < max_marching_steps; i++) {
        float dist = SDF(r.origin + depth * r.direction);
        if (dist < epsilon) {
            return bias;
        }
        depth += dist;
        res = min(res, k * dist / depth + bias);
    }
    res = clamp( res, 0.0, 1.0 );
    return res*res*(3.0-2.0*res);
}

vec3 calc_normal(vec3 p )
{
    const float h = 0.0001;
    const vec2 k = vec2(1,-1);
    return normalize( k.xyy*SDF( p + k.xyy*h ) +
    k.yyx*SDF( p + k.yyx*h ) +
    k.yxy*SDF( p + k.yxy*h ) +
    k.xxx*SDF( p + k.xxx*h ) );
}

void main() {
    vec3 light_position = vec3(2., 2., 2);
    float light_bias = 0.0;
    
    int rm_steps = 250;
    float rm_start = 0.;
    float rm_end = 100.;
    float rm_eps = 0.0001;
    
    float aspect_ratio = iResolution.x / iResolution.y;
    vec2 uv = fragCoord + 0.5;

    camera c = camera(cam_from, cam_at, vec3(0., 1., 0.), 60.);
//    camera c = camera(vec3(0., 1. + 2 * abs(cos(iTime)), 5. - 2 * sin(iTime)), vec3(0., 0., 0.), vec3(0., 1., 0.), 20.);
    vec3 dir = get_ray_direction(c, aspect_ratio, uv);
//    vec3 dir = vec3(uv, 1.);
    vec2 rmr_result = ray_march(ray(c.lookfrom, dir), rm_steps, rm_start, rm_end, rm_eps);
    float rmr = rmr_result.x;
    float rmr_iter_count = rmr_result.y;
    if (rmr >= rm_end - rm_eps) {
        fragColor = vec4(0., 0., 0., 1.);
        return;
    }
    vec3 intersection_p = c.lookfrom + dir * rmr;
    vec3 ip_normal = calc_normal(intersection_p); 
    vec3 light_diff = light_position - intersection_p;
    float light_dist = length(light_diff);
    float light_strength = 2.;
    vec3 light_color = vec3(0.5,0.5,0.5);
//    float shadow_rmr = shadow_ray_march(ray(intersection_p, normalize(light_diff)), rm_steps, 0.05, light_dist, rm_eps * rm_eps, 8., light_bias);
//    shadow_rmr = 1.;
//    float diffuse = (light_bias + clamp(dot(calc_normal(intersection_p), light_diff) + 0., 0., 1.)) / (light_dist * light_dist) * light_strength;
    vec3 material = vec3(0.3);
//    vec3 hal = normalize(light_position - dir);
//    vec3 reflect_dir = reflect(-normalize(light_diff), ip_normal);
//    float diffuse = shadow_rmr * (light_bias + clamp(dot(ip_normal, light_diff), 0., 1.0));
//    float specular = pow(clamp(dot(ip_normal, hal), 0., 1.), 32.) *
//        diffuse * (0.04 + 0.96*pow( clamp(1.0+dot(hal, dir),0.0,1.0), 5.0 ));
//    vec3 col = material * 4. * diffuse * light_color;
//    col += 12. * specular * light_color;
    
//    float occ = 1. - rmr_iter_count / float(rm_steps);
//    float amb = clamp(0.5 + 0.5 * ip_normal.y, 0., 1.);
    
//    col += material * amb * occ * vec3(1.,1.,1.);
//    col *= exp( -0.0005*rmr*rmr*rmr );
//    col = pow( col, vec3(0.4545) );
    vec3 blue = vec3(0., 0.34, 0.71);
    vec3 yellow = vec3(1., 0.87, 0.);
    float l = clamp(float(rmr_iter_count) / 40., 0., 1.);
    vec3 clr_coef = vec3(l);
    vec3 clr = (1 - dir.y) * blue + dir.y * yellow;
//    vec3 clr = (1 - l) * yellow + l * blue;
    vec3 result_color = clr * clr_coef;
//    vec3 result_color = clr;
    fragColor = vec4(result_color, 1.0);  
//    fragColor = vec4((shadow_rmr) / 2 * vec3(1.), 1.);  

}