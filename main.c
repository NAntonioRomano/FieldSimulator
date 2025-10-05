#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// --- Constantes Físicas y de Configuración ---
#define k (8.987e9)
#define DIM_A 32 // Nodos en el eje X
#define DIM_B 24 // Nodos en el eje Y

// --- Configuración de la Ventana y Malla ---
const int screenWidth = 800;
const int screenHeight = 600;
const int gridSpacing = 25;
const int initPadding = 12;

// --- Estructuras de Datos ---

typedef struct {
    Vector2 position;
    float Ex, Ey;
    float V;
} MeshPoint;

typedef struct {
    Vector2 position;
    float Q;
    bool isDragging;
} PointCharge;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    Vector2 aceleration;
    float m,q;
}massCharge;


// --- Prototipos de Funciones ---
void DrawVectors(MeshPoint mesh[][DIM_B]);
void DrawPointCharges(PointCharge p[], int n);
void initMesh(MeshPoint m[][DIM_B]);
Vector2 getElectricField(PointCharge p[], int n, Vector2 position);
void changeElectricField(MeshPoint m[][DIM_B], PointCharge p[], int n);
Vector2 getElectricForze(PointCharge p[], int n, float q, Vector2 position);
void changeAceleration(PointCharge p[],int n, massCharge *m);


int main(void) {
    // --- 1. CONFIGURACIÓN DE CARGAS FUENTE ---
    PointCharge charges[] = {
        { .position = {.x = screenWidth/2, .y = screenHeight/2}, .Q = -1e-12f },
        { .position = {.x = screenWidth/4, .y = screenHeight/4}, .Q = 1e-12f }
    };
    int numCharges = sizeof(charges) / sizeof(charges[0]);

    // --- 2. CONFIGURACIÓN DE LA MALLA ---
    MeshPoint mesh[DIM_A][DIM_B];
    initMesh(mesh);

    // --- 3. CREAR LA PARTÍCULA MÓVIL (¡NUEVO!) ---
    massCharge particle = {
        .position = { (float)screenWidth / 4, (float)screenHeight / 4 },
        .velocity = { 0.0f, 0.0f },
        .aceleration = { 0.0f, 0.0f },
        .m = 1.0e-19f,  // Masa muy pequeña (ej. un ion)
        .q = 1.6e-10f, // Carga positiva pequeña (ej. un protón)
    };

    // --- 4. INICIALIZACIÓN DE LA VENTANA ---
    InitWindow(screenWidth, screenHeight, "Simulador de Campo Eléctrico");
    SetTargetFPS(120);

    // --- BUCLE PRINCIPAL ---
    while (!WindowShouldClose()) {
        float dt = GetFrameTime(); // Delta time para física fluida

        // --- ACTUALIZACIÓN DE ESTADO ---
        // a. Calculamos la aceleración actual de la partícula
        // Pasamos la dirección de memoria con el operador '&'
        changeAceleration(charges, numCharges, &particle); 


        
        // b. Actualizamos su velocidad usando la aceleración
        particle.velocity.x += particle.aceleration.x * dt;
        particle.velocity.y += particle.aceleration.y * dt;

        // c. Actualizamos su posición usando la velocidad
        particle.position.x += particle.velocity.x * dt;
        particle.position.y += particle.velocity.y * dt;

        // --- AÑADE ESTE BLOQUE PARA BORDES SÓLIDOS ---

        // Comprobación del borde izquierdo
        if (particle.position.x <= 0) {
            particle.position.x = 0; // La "pegamos" al borde
            if (particle.velocity.x < 0) { // Si todavía se mueve hacia la izquierda...
                particle.velocity.x = 0;   // ...frenamos su velocidad horizontal.
            }
        }

        // Comprobación del borde derecho
        if (particle.position.x >= screenWidth) {
            particle.position.x = screenWidth;
            if (particle.velocity.x > 0) { // Si todavía se mueve hacia la derecha...
                particle.velocity.x = 0;
            }
        }

        // Comprobación del borde superior
        if (particle.position.y <= 0) {
            particle.position.y = 0;
            if (particle.velocity.y < 0) { // Si todavía se mueve hacia arriba...
                particle.velocity.y = 0;   // ...frenamos su velocidad vertical.
            }
        }

        // Comprobación del borde inferior
        if (particle.position.y >= screenHeight) {
            particle.position.y = screenHeight;
            if (particle.velocity.y > 0) { // Si todavía se mueve hacia abajo...
                particle.velocity.y = 0;
            }
        }
        // ---------------------------------------------

        // d. Actualizamos la malla del campo eléctrico (visual)
        changeElectricField(mesh, charges, numCharges);

        // --- DIBUJADO ---
        BeginDrawing();
            ClearBackground(DARKGRAY);
            
            // Dibujar todo lo que ya tenías
            DrawVectors(mesh);
            DrawPointCharges(charges, numCharges);
            
            // Dibujar la partícula móvil (¡NUEVO!)
            DrawCircleV(particle.position, 5, (particle.q > 0) ? ORANGE : SKYBLUE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// --- Implementación de Funciones ---

void DrawVectors(MeshPoint mesh[][DIM_B]){
    // Constantes para controlar el aspecto de las flechas
    const float arrowLength = 10.0f; // Longitud de la línea principal de la flecha
    const float arrowHeadSize = 5.0f; // Longitud de las "alas" de la punta
    const float arrowHeadAngle = PI / 6; // Ángulo de las "alas" (30 grados)


    for (int i = 0; i < DIM_A; i++) {
        for (int j = 0; j < DIM_B; j++) {
            // Obtenemos el vector del campo eléctrico
            float Ex = mesh[i][j].Ex;
            float Ey = mesh[i][j].Ey;

            // Calculamos la magnitud (longitud) del vector
            float magnitude = sqrtf(Ex * Ex + Ey * Ey);

            // Solo dibujamos si el campo no es cero (para evitar división por cero)
            if (magnitude > 1e-9) {
                // 1. NORMALIZAMOS EL VECTOR (lo hacemos de longitud 1)
                float unitX = Ex / magnitude;
                float unitY = Ey / magnitude;

                // Posición inicial de la flecha (el punto de la malla)
                float startX = mesh[i][j].position.x;
                float startY = mesh[i][j].position.y;

                // Posición final (la punta de la flecha) usando la longitud fija
                float endX = startX + unitX * arrowLength;
                float endY = startY + unitY * arrowLength;

                // Dibujamos la línea principal de la flecha
                DrawLineEx((Vector2){startX, startY}, (Vector2){endX, endY}, 1.5f, GREEN);

                // 2. DIBUJAMOS LA PUNTA DE LA FLECHA
                // Obtenemos el ángulo de la línea principal
                float lineAngle = atan2f(unitY, unitX);
                
                // Calculamos el ángulo para la primera "ala" de la punta
                float angle1 = lineAngle + PI - arrowHeadAngle;
                Vector2 p1 = {
                    endX + arrowHeadSize * cosf(angle1),
                    endY + arrowHeadSize * sinf(angle1)
                };

                // Calculamos el ángulo para la segunda "ala"
                float angle2 = lineAngle + PI + arrowHeadAngle;
                Vector2 p2 = {
                    endX + arrowHeadSize * cosf(angle2),
                    endY + arrowHeadSize * sinf(angle2)
                };
                
                // Dibujamos las dos líneas que forman la punta
                DrawLineV((Vector2){endX, endY}, p1, GREEN);
                DrawLineV((Vector2){endX, endY}, p2, GREEN);
            }
        }
    }
}

void DrawPointCharges(PointCharge p[], int n){
    Vector2 mousePosition = GetMousePosition();
    // DIBUJAR LAS CARGAS FUENTE
    for (int i = 0; i < n; i++) {
        Color chargeColor = (p[i].Q > 0) ? RED : BLUE; // Rojas si son +, Azules si son -
        if (CheckCollisionPointCircle(mousePosition, p[i].position, 8) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            p[i].isDragging = true; // Si es así, activamos el modo "arrastrar"
        }

        // Si estamos en modo "arrastrar"...
        if (p[i].isDragging){
            p[i].position = mousePosition; // ...la posición del círculo es la del ratón.
        }

        // Comprobamos si se ha soltado el botón izquierdo del ratón
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
            p[i].isDragging = false; // Si es así, desactivamos el modo "arrastrar"
        }
        DrawCircle(p[i].position.x, p[i].position.y, 8, chargeColor);
    }
}

void initMesh(MeshPoint m[][DIM_B]) {
    int i = 0;
    for (int x = initPadding; x < screenWidth && i < DIM_A; x += gridSpacing) {
        int j = 0;
        for (int y = initPadding; y < screenHeight && j < DIM_B; y += gridSpacing) {
            m[i][j].position.x = x;
            m[i][j].position.y = y;
            m[i][j].V = 0;
            m[i][j].Ex = 0;
            m[i][j].Ey = 0;
            j++;
        }
        i++;
    }
}

Vector2 getElectricField(PointCharge p[], int n, Vector2 position){
    Vector2 E = { 0.0f, 0.0f };
        for(int i = 0; i < n; i++){
            float dx = position.x - p[i].position.x;
            float dy = position.y - p[i].position.y;
            float dist_sq = dx * dx + dy * dy;

            if (dist_sq > 1e-6) {
                float dist = sqrt(dist_sq);
                float E_magnitude = k * p[i].Q / dist_sq;

                E.x += E_magnitude * dx / dist;
                E.y += E_magnitude * dy / dist;
        }
    }

    return E;
}

void changeElectricField(MeshPoint m[][DIM_B], PointCharge p[], int n) {
    for (int i = 0; i < DIM_A; i++) {
        for (int j = 0; j < DIM_B; j++) {
            m[i][j].Ex = 0.0f;
            m[i][j].Ey = 0.0f;
        }
    }

    Vector2 E;

    for (int i = 0; i < DIM_A; i++) {
        for (int j = 0; j < DIM_B; j++) {
            E = getElectricField(p,n, m[i][j].position);
            m[i][j].Ex = E.x;
            m[i][j].Ey = E.y;
        }
    }
}

Vector2 getElectricForze(PointCharge p[], int n, float q, Vector2 position){
    Vector2 F = { 0.0f, 0.0f };
    Vector2 E = getElectricField(p,n,position);
    F.x += E.x * q;
    F.y += E.y * q;
    
    return F;
}

void changeAceleration(PointCharge p[],int n, massCharge *m){
    Vector2 F = getElectricForze(p,n,m->q, m->position);

    m->aceleration.x = F.x /m->m;
    m->aceleration.y = F.y /m->m;
}