#ifndef MAT4_H
#define MAT4_H

#include "vec4.h"
#include <cmath>
#include <iostream>

class mat4 {
    public:
        double m[4][4]; // m[linha][coluna]

        // Construtor: Identidade (Diagonal = 1, resto = 0)
        mat4() {
            for(int i=0; i<4; i++)
                for(int j=0; j<4; j++)
                    m[i][j] = (i == j) ? 1.0 : 0.0;
        }

        // Acesso aos elementos
        double* operator[](int i) { return m[i]; }
        const double* operator[](int i) const { return m[i]; }

        // --- Operações Estáticas de Criação de Matrizes ---

        // 1. Matriz de Translação
        // Move o ponto por (v.x, v.y, v.z)
        static mat4 translate(const vec3& v) {
            mat4 mat;
            mat[0][3] = v.x();
            mat[1][3] = v.y();
            mat[2][3] = v.z();
            return mat;
        }

        // 2. Matriz de Escala
        // Multiplica as dimensões por (v.x, v.y, v.z)
        static mat4 scale(const vec3& v) {
            mat4 mat;
            mat[0][0] = v.x();
            mat[1][1] = v.y();
            mat[2][2] = v.z();
            return mat;
        }

        // 3. Matriz de Rotação (Eixo Z)
        static mat4 rotate_z(double angle_radians) {
            mat4 mat;
            double c = cos(angle_radians);
            double s = sin(angle_radians);
            mat[0][0] = c; mat[0][1] = -s;
            mat[1][0] = s; mat[1][1] = c;
            return mat;
        }

        // 3. Matriz de Rotação (Eixo Y)
        static mat4 rotate_y(double angle_radians) {
            mat4 mat;
            double c = cos(angle_radians);
            double s = sin(angle_radians);
            mat[0][0] = c;  mat[0][2] = s;
            mat[2][0] = -s; mat[2][2] = c;
            return mat;
        }

        // 3. Matriz de Rotação (Eixo X)
        static mat4 rotate_x(double angle_radians) {
            mat4 mat;
            double c = cos(angle_radians);
            double s = sin(angle_radians);
            mat[1][1] = c; mat[1][2] = -s;
            mat[2][1] = s; mat[2][2] = c;
            return mat;
        }

        // --- Operações Algébricas ---

        // Multiplicação Matriz x Matriz (Combina transformações)
        // Lembre-se: A ordem importa! T * R (Gira depois Translada) != R * T
        mat4 operator*(const mat4& other) const {
            mat4 result;
            for(int i=0; i<4; i++) {
                for(int j=0; j<4; j++) {
                    result[i][j] = 0;
                    for(int k=0; k<4; k++) {
                        result[i][j] += m[i][k] * other[k][j];
                    }
                }
            }
            return result;
        }

        // Multiplicação Matriz x Vetor (Aplica a transformação ao ponto/vetor)
        vec4 operator*(const vec4& v) const {
            double res[4];
            for(int i=0; i<4; i++) {
                res[i] = 0;
                for(int j=0; j<4; j++) {
                    res[i] += m[i][j] * v[j];
                }
            }
            return vec4(res[0], res[1], res[2], res[3]);
        }
        
        // Inversa (Simplificada ou Geral)
        // Para Ray Tracing, precisamos da inversa para trazer o Raio do Mundo para o Espaço do Objeto.
        // Implementar a inversa completa de 4x4 é complexo (Gauss-Jordan), 
        // mas para transformações afins básicas, podemos expandir depois se necessário.
        // Por enquanto, vamos assumir que o usuário usará bibliotecas ou implementará se precisar de cisalhamento complexo.
        // Dica: Para Rotação+Translação, a inversa é fácil calcular separadamente.
};

#endif