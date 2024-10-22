#include<iostream>

#include "C:\mingw64\include\c++\14.1.0\raylib.h"
#include<deque>
#include<string>

using namespace std;

const int cellSize = 30;
const int cellCount = 25;
const int offset = 75;

Color green = {173, 204, 96,255};
Color dark_green = {43, 51,24,255};
Color red = {255,0,0,255};
float event_speed = 0.2;

double lastUpdateTime = 0;


bool eventTrigger(double interval){
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake{
public:
    deque<Vector2> body;
    Vector2 direction = {1,0};
    bool addSegment = false;

    Snake(){
        body={Vector2{6,9},Vector2{5,9},Vector2{4,9}};
    }

    void draw(){
        for(unsigned int i=0;i<body.size();i++){
            float x=body[i].x;
            float y=body[i].y;
            Rectangle segment=Rectangle{offset+x*cellSize,offset+y*cellSize,(float)cellSize,(float)cellSize};
            DrawRectangleRounded(segment,0.5,6,dark_green);
        }
    }

    void update(){
        if(addSegment == true){
            body.push_front(Vector2{body[0].x+direction.x,body[0].y+direction.y});
            event_speed-=0.005;
            if(event_speed<0.1){
                event_speed=0.1;
            }
            addSegment=false;
        }
        else{
            body.pop_back();
            body.push_front(Vector2{body[0].x+direction.x,body[0].y+direction.y});
        }
    }

    void reset(){
        body = {Vector2{6,9},Vector2{5,9},Vector2{4,9}};
        direction = {1,0};
    }

    
};
class Food{
public:
    Vector2 position;
    deque<Vector2> snakeBody;

    Food(deque<Vector2> body){
        this->snakeBody=body;
        position=GetRandomPos();

    }

    void updateSnakeBody(deque<Vector2> body){
        snakeBody=body;
    }

    void draw(){
        DrawRectangle(offset+position.x*cellSize,offset+position.y*cellSize,cellSize,cellSize,red);
    }

    bool isFoodBodyCollide(deque<Vector2> body,Vector2 position){
        for(unsigned int i = 0;i<body.size();i++ ){
            if(body[i].x==position.x && body[i].y==position.y){
                return true;
            }
        }
        return false;
    }

    Vector2 GenerateRandomCell(){
        float x = GetRandomValue(0,cellCount-1);
        float y = GetRandomValue(0,cellCount-1);
        return Vector2{x,y};
    }

    Vector2 GetRandomPos(){
        Vector2 position = GenerateRandomCell();
        while(isFoodBodyCollide(snakeBody,position)){
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    Sound death_sound;
    Sound eat_sound;
    Game(){
        InitAudioDevice();
        death_sound = LoadSound("Uuhhh.mp3");
        eat_sound = LoadSound("crunch.mp3");
    }
   
    void draw(){
        snake.draw();
        food.draw();
    }

    void update(){
        if(running){
            snake.update();
            checkFoodBodyCollision();
            checkCollisionWithEdges();
            checkHeadCollision();
            food.updateSnakeBody(snake.body);
        }
    }

    void checkFoodBodyCollision(){
        if(snake.body[0].x==food.position.x && snake.body[0].y==food.position.y){
            food.position=food.GetRandomPos();
            PlaySound(eat_sound);
            snake.addSegment=true;
        }
    }

    void checkCollisionWithEdges(){
        if(snake.body[0].x==-1){
            snake.body[0].x=24;
        }
        if(snake.body[0].x==25){
            snake.body[0].x=0;
        }
        if(snake.body[0].y==-1){
            snake.body[0].y=24;
        }
        if(snake.body[0].y==25){
            snake.body[0].y=0;
        }
    }

    void checkHeadCollision(){
        for(unsigned int i=1;i<snake.body.size();i++){
            if(snake.body[0].x==snake.body[i].x && snake.body[0].y==snake.body[i].y){
                GameOver();
            }
        }
    }

    void GameOver(){
        snake.reset();
        running=false;
        event_speed=0.2;
        PlaySound(death_sound);
    }
    ~Game(){
        UnloadSound(death_sound);
        UnloadSound(eat_sound);
        CloseAudioDevice();
    }
};

int main(){
    InitWindow(2*offset+cellSize*cellCount,2*offset+cellCount*cellSize,"Snake Game");
    
    SetTargetFPS(60);
    Game game=Game();
    while(WindowShouldClose()==false){
        
        BeginDrawing();
        if(eventTrigger(event_speed)){
            game.update();
        }
        if(IsKeyPressed(KEY_UP) && game.snake.direction.y!=1){
            game.snake.direction={0,-1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y!=-1){
            game.snake.direction={0,1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x!=1){
            game.snake.direction={-1,0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x!=-1){
            game.snake.direction={1,0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_P)){
            if(game.running){
                game.running=false;
            }
            else{
                game.running=true;
            }
        }
        
        ClearBackground(green);
        string display = "Score:"+to_string(game.snake.body.size()-3);
        char display1[display.size()+1];
        for(int i=0;i<display.size();i++){
            display1[i]=display[i];
        }
        display1[display.size()]='\0';
        DrawText(display1,75,0,50,dark_green);
        game.draw();
        DrawRectangleLinesEx(Rectangle{offset-5,offset-5,cellSize*cellCount+10,cellSize*cellCount+10},5,dark_green);        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}