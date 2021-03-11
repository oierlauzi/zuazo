//This code is based on
//https://developer.nvidia.com/gpugems/gpugems3/part-iv-image-effects/chapter-25-rendering-vector-art-gpu

//The algebraic form of the quadratic bezier curve is:
//k^2 - lm = 0
//where k=x, l=y, m=1

float bezier2_distance(vec2 kl) {
	//Apply the equation described above
	return kl.x*kl.x - kl.y;
}

vec2 bezier2_gradient(vec2 kl) {
	//Obtain the jacobian matrix of the procedural texture coordinates
	const mat2x2 jacobian = transpose(mat2x2(dFdx(kl), dFdy(kl)));

	//Obtain the vector for applying the chain rule:
	//[f(g(x))]' = f'(g(x))*g'(x)
	const vec2 v = vec2(2.0f*kl.x, -kl.y); //2u, -v
	
	//Obtain the gradient
	return jacobian*v;
}

float bezier2_signed_distance(vec2 kl) {
	return bezier2_distance(kl) / length(bezier2_gradient(kl));
}


//The algebraic form of the cubic bezier curve is:
//k^3 - lmn = 0
//where k=x, l=y, m=z, n=1

float bezier3_distance(vec3 klm) {
	//Apply the equation described above
	return klm.x*klm.x*klm.x - klm.y*klm.z;
}

vec2 bezier3_gradient(vec3 klm) {
	//Obtain the jacobian matrix of the procedural texture coordinates
	const mat3x2 jacobian = transpose(mat2x3(dFdx(klm), dFdy(klm)));

	//Obtain the vector for applying the chain rule and multiplication derivation property
	//[f(g(x))]' = f'(g(x))*g'(x)
	//[f(x)*g(x)]' = f'(x)*g(x) + g'(x)*f(x)
	const vec3 v = vec3(3.0f*klm.x*klm.x, -klm.z, -klm.y); //3k^2, -m, -l
	
	//Obtain the gradient
	return jacobian*v;
}

float bezier3_signed_distance(vec3 klm) {
	return bezier3_distance(klm) / length(bezier3_gradient(klm));
}
