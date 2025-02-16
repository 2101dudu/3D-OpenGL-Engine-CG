#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

// Estrutura para representar um ponto 3D
struct Point {
    float x, y, z;

    Point(float x_val = 0.0f, float y_val = 0.0f, float z_val = 0.0f)
        : x(x_val), y(y_val), z(z_val) {}
};

// Estrutura para representar uma associação entre 3 pontos (triângulos)
struct Association {
    int p1, p2, p3;
};

// Função para analisar o arquivo e retornar os pontos dos triângulos
std::vector<Point> parseFile(const std::string& filename);

// Função para imprimir os pontos no terminal
void printPoints(const std::vector<Point>& points);

#endif