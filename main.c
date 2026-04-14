#include <raylib.h>
#include <stdio.h>
#include <math.h>

#define PI 3.1415926535
#define PI2 PI/2
#define PI3 3*PI/2
#define DR 0.0174532925 // one degree in radians


float playerX, playerY, playerDeltaX, playerDeltaY, playerAngle; //player Angel of our players view, delta to calculate a movement

int mapX = 8, mapY = 8, mapS=64;

int map[]=
{
    1,1,1,1,1,1,1,1,
    1,0,1,0,0,0,0,1,
    1,0,2,0,0,1,0,1,
    1,0,0,0,0,0,0,1,
    1,0,2,0,0,1,0,1,
    1,0,0,0,3,1,0,1,
    1,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,
};

void drawMap2D() {
    int x, y, xOffset, yOffset;
    for (y = 0; y < mapY; y++) {
        for (x = 0; x < mapX; x++) {
            xOffset = x * mapS;
            yOffset = y * mapS;

            int tile = map[y * mapX + x];

            Color wallColor;

            if (tile == 1) {wallColor = WHITE;}
            else if (tile == 2) {wallColor = RED;}
            else if (tile == 3) {wallColor = BLUE;}
            else {wallColor = BLACK;}

            DrawRectangle(xOffset, yOffset, mapS - 1, mapS - 1, wallColor);
        }
    }
}

void init() {
    InitWindow(1024, 512, "RayCast");
    SetTargetFPS(60);

    playerX = 300, playerY = 300; playerDeltaX=cos(playerAngle); playerDeltaY = sin(playerAngle); //player position
}

void drawPlayer() {
    DrawRectangle((int)playerX, (int)playerY, 8, 8, YELLOW);

    float centerX = playerX + 4; // center of a player
    float centerY = playerY + 4;

    Vector2 startPos = { centerX, centerY };
    Vector2 endPos   = { centerX + playerDeltaX * 20, centerY + playerDeltaY * 20 };

    DrawLineEx(startPos, endPos, 3.0f, YELLOW);
}


float dist(float startX, float startY, float endX, float endY) {
    return(sqrt((endX-startX)*(endX-startX) + (endY-startY)*(endY-startY)));
}


void drawRays3D() {
    int r, targetColumn, targetRow, cellIndex, depthOfField;
    float rayX, rayY, rayAngle, xOffset, yOffset, disT; // r = Ray, o = Offset
    Color colour;
    int horizontal_map_value, vertical_map_value;
    rayAngle = playerAngle-DR*30; // player Angel

    if(rayAngle<0) { rayAngle+= 2*PI; }
    if(rayAngle> 2*PI) { rayAngle -= 2*PI; }



    for (r = 0; r < 60; r++) {
        // Horizontall check
        depthOfField = 0;
        float distanceH = 1000000; // closest wall checker
        float horizHitX = playerX, horizHitY = playerY;
        float Ncotangent = -1 / tan(rayAngle);

        // if ray looking up
        if (rayAngle > PI) {
            rayY = (((int)playerY >> 6) << 6) - 0.0001f;
            rayX = (playerY - rayY) * Ncotangent + playerX;
            yOffset = -64;
            xOffset = -yOffset * Ncotangent;
        }
        // looking down
        else if (rayAngle < PI && rayAngle > 0) {
            rayY = (((int)playerY >> 6) << 6) + 64;
            rayX = (playerY - rayY) * Ncotangent + playerX;
            yOffset = 64;
            xOffset = -yOffset * Ncotangent;
        }
        // left or right
        else {
            rayX = playerX;
            rayY = playerY;
            depthOfField = 8;
        }

        while (depthOfField < 8) {
            targetColumn = (int)(rayX) >> 6;
            targetRow = (int)(rayY) >> 6;
            cellIndex = targetRow * mapX + targetColumn;

            // if we hit the wall
            if (cellIndex >= 0 && cellIndex < mapX * mapY && map[cellIndex] > 0) {
                horizontal_map_value = map[cellIndex];
                horizHitX = rayX;
                horizHitY = rayY;
                distanceH = dist(playerX, playerY, horizHitX, horizHitY);
                depthOfField = 8;
            } else {
                rayX += xOffset;
                rayY += yOffset;
                depthOfField += 1;
            }
        }
        // Drowing the line from the center of our player for the hit
        // DrawLineEx((Vector2){px + 4, py + 4}, (Vector2){rx, ry}, 2.0f, RED);


    //vertical
    depthOfField = 0;
    float distantV = 1000000; // closest wall checker
    float vertHitX = playerX, vertHitY = playerY;
    float NTangent = -tan(rayAngle);

    // Смотрим ВЛЕВО
    if (rayAngle > PI2 && rayAngle < PI3) {
        rayX = (((int)playerX >> 6) << 6) - 0.0001f;
        rayY = (playerX - rayX) * NTangent + playerY;
        xOffset = -64;
        yOffset = -xOffset * NTangent;
    }
    // Смотрим ВПРАВО
    else if (rayAngle < PI2 || rayAngle > PI3) {
        rayX = (((int)playerX >> 6) << 6) + 64;
        rayY = (playerX - rayX) * NTangent + playerY;
        xOffset = 64;
        yOffset = -xOffset * NTangent;
    }
    else {
        rayX = playerX;
        rayY = playerY;
        depthOfField = 8;
    }

    while (depthOfField < 8) {
        targetColumn = (int)(rayX) >> 6;
        targetRow = (int)(rayY) >> 6;
        cellIndex = targetRow * mapX + targetColumn;

        if (cellIndex >= 0 && cellIndex < mapX * mapY && map[cellIndex] > 0) {
            vertical_map_value = map[cellIndex];
            vertHitX = rayX;
            vertHitY = rayY;
            distantV = dist(playerX, playerY, vertHitX, vertHitY);
            depthOfField = 8;
        } else {
            rayX += xOffset;
            rayY += yOffset;
            depthOfField += 1;
        }
    }

    Color wallColor;
    int wallType = 0;

    if(distantV < distanceH) {rayX = vertHitX; rayY = vertHitY; disT = distantV; wallType = vertical_map_value;}
    if(distanceH < distantV) {rayX = horizHitX; rayY = horizHitY; disT = distanceH; wallType = horizontal_map_value;}

    // checking wall color
    switch(wallType) {
    case 1: wallColor = WHITE; break;
    case 2: wallColor = RED;   break;
    case 3: wallColor = BLUE;  break;
    default: wallColor = WHITE; break;
    }

    if (distanceH < distantV) {
    wallColor.r *= 0.7;
    wallColor.g *= 0.7;
    wallColor.b *= 0.7;
    }

    DrawLineEx((Vector2){playerX + 4, playerY + 4}, (Vector2){rayX, rayY}, 2.0f, wallColor);

    // 3D part
    float correctAngle = playerAngle-rayAngle;
    if(correctAngle<0) { correctAngle+= 2*PI; }
    if(correctAngle> 2*PI) { correctAngle -= 2*PI;}
    disT = disT * cos(correctAngle); // fix fish eye

    float lineHeight = (mapS*320)/disT;
    if(lineHeight>320){lineHeight = 320;}
    float lineOffset = 160 -lineHeight/2;
    DrawLineEx((Vector2){r*8 + 530, lineOffset}, (Vector2){r*8 + 530, lineHeight+lineOffset}, 8.0f, wallColor);


    rayAngle += DR;
    if(rayAngle<0) { rayAngle+= 2*PI; }
    if(rayAngle> 2*PI) { rayAngle -= 2*PI; }
    }
}



void keybind_player() {
    if (IsKeyDown(KEY_RIGHT)) { playerAngle += 0.1; if (playerAngle>2*PI)  playerAngle -= 2*PI;} {playerDeltaX=cos(playerAngle); playerDeltaY = sin(playerAngle); };
    if (IsKeyDown(KEY_LEFT))  { playerAngle -= 0.1; if (playerAngle<0)  playerAngle += 2*PI;} {playerDeltaX=cos (playerAngle); playerDeltaY = sin(playerAngle); };
    if (IsKeyDown(KEY_UP))    {playerX += playerDeltaX; playerY += playerDeltaY;};
    if (IsKeyDown(KEY_DOWN))  {playerX -= playerDeltaX; playerY -= playerDeltaY;};
}

int main(void) {
    init();

    while (!WindowShouldClose()) {

        keybind_player();

        BeginDrawing();

            ClearBackground(GRAY);
            drawMap2D();
            drawRays3D();
            drawPlayer();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}