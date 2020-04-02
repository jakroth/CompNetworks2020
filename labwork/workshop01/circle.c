#include <stdio.h>

#define PI 3.141592

double area(double radius);
double circumference(double radius);

int main(int argc, const char *argv[]) {
    printf("Enter a radius in metres: ");
    double radius = 0.0;
    const int result = scanf("%lf", &radius);

    // True if the user's value was assigned to radius
    if (result > 0) {
        printf("Area: %.2lfm\n", area(radius));
        printf("Circumference: %.2lfm\n", circumference(radius));
    }
    
    return 0;
}

double area(double radius) {
    return PI * radius * radius;
}

double circumference(double radius){
    return 2 * PI * radius;
}