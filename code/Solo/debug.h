int counter = 0;
float tempo = 0;

float debugVelocityY()
{
    return 10 * std::sin(counter * 3.14 / 8);
}

float debugVelocityZ()
{
    return 8 * std::cos(counter * 3.14 / 8);
}

float debugAccelerationX()
{
    return pow(counter, 2);
}

float debugAccelerationY()
{
    return 10 * std::sin(counter * 3.14 / 8) + 10 * std::cos(counter * 3.14 / 8);
}