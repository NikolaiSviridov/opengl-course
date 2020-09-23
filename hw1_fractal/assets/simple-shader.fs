#version 330 core

uniform sampler1D u_texture;
uniform int u_iterations;
uniform vec2 u_random_value;
uniform vec2 u_offset;
uniform float u_zoom;

in vec2 v_out;
out vec4 o_frag_color;

vec2 f(vec2 n) {
    return vec2(
            n.x * n.x * n.x - 3.0 * n.x * n.y * n.y - 1.0,
            -n.y * n.y * n.y + 3.0 * n.x * n.x * n.y
    );
}

vec2 df(vec2 n) {
    return 3.0 * vec2(
            n.x * n.x - n.y * n.y,
            2.0 * n.x * n.y
    );
}

vec2 cdiv(vec2 a, vec2 b) {
    float d = dot(b, b);
    if (d == 0.0) return a;
    else
        return vec2(
                (a.x * b.x + a.y * b.y) / d,
                (a.y * b.x - a.x * b.y) / d
        );
}

void main() {
    vec2 z = v_out.xy  * u_zoom + u_offset;

    int i;
    for (i = 0; i != u_iterations; ++i) {
        vec2 zn = z - cdiv(f(z), df(z)) + u_random_value;
        if(distance(zn, z) < 0.00001) break;
        z = zn;
    }

   o_frag_color = texture(u_texture, float(i) / u_iterations);
}
