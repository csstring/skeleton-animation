#version 330 core

// input
in vec4 fragPosition;
// Ouput data
out vec4 color;

void main()
{ 
	if (fragPosition.y > 0.01)
    {
        // Assign fixed color for z == 0
        color = vec4(0.1882, 0.7608, 0.1373, 1.0);  // Yellow
    }
    else
    {
        // Calculate color based on other conditions or positions
        vec3 mixColor1 = vec3(1, 0, 0); // Base color (blue)
        vec3 mixColor2 = vec3(0, 1, 0);  // Color to mix (red)
		vec3 mixColor3 = vec3(0, 0, 1);  // Color to mix (red)


        // Mix the colors based on the position
        vec3 finalColor = mix(mixColor1, mixColor2, fragPosition.x);
		finalColor = mix(finalColor, mixColor3, fragPosition.z);

        // Output the final color
        color = vec4(finalColor, 1.0);
    }
}