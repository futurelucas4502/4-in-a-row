#include <SFML/Graphics.h>
#include <SFML/System.h>
#include <SFML/Window.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Preprocess function types
void userClick(int xPos, int yPos);
bool clickCircle(sfVector2f clickPos);
bool clickRectangle(sfVector2f clickPos);
void label(const char *userText, int x, int y, int fontSize, sfColor textColour);
void button(sfRectangleShape *rectangle, int rectangleWidth, int rectangleHeight, int rectangleX, int rectangleY, sfColor rectangleColour, const char *userText, int fontSize, sfColor textColor);
void winCheck();
void shapeFill();
void gameOver();

// This is the actual Render window its defined like this as it is a pointer to an sfRenderWindow that we will then interact with
sfRenderWindow *window;
// Create circle array each circle is a seperate shape object so each must be interacted with individually
sfCircleShape *circle[7][6];
sfRectangleShape *startButton;
sfRectangleShape *quitButton;
// Define user1sTurn as this will allow us to choose which colour to set the circle to based on the users turn
bool user1sTurn = true;
bool gameRunning = false;
// Define text info
sfFont *textFont;
sfText *text;
// Create title font and text
sfFont *titleFont;
sfText *title;
char *startButtonText;
char *infoLabelText;

// TODO: Maybe add some kind of try catch system so if it errors anywhere we clear all memory before closing as this probably has memory leaks in its current state

int main(int argc, char const *argv[]) {
    // Main setup of window and event handling
    // This will be used in the main loop its used to keep track of when certain events take place and lets us do things like event listeners (kinda)
    sfEvent event;
    // Default bit colour depth at set resolution
    sfVideoMode mode = {1000, 700, sfVideoMode_getDesktopMode().bitsPerPixel};
    // Start creating the window i could use sfVideoMode_getDesktopMode() instead of mode however this is like the max ur screen can do and idk why ud ever want this so
    window = sfRenderWindow_create(mode, "4 in a row", sfResize | sfClose, NULL);
    if (!window) {
        puts("Unable to create window, aborting.");
        return 1;
    }

    // Set frame rate to 30fps to reduce CPU usage
    sfRenderWindow_setFramerateLimit(window, 30);

    // End main setup of window and event handling

    // Create font from file so we can use it for our title this is done outside of our game loop to prevent importing the font 30 times a second...
    titleFont = sfFont_createFromFile("Showdex.otf");
    if (!titleFont) {
        puts("Unable to create title font, aborting.");
        sfRenderWindow_destroy(window);
        free(startButtonText);
        return 1;
    }
    title = sfText_create();
    sfText_setString(title, "Welcome to 4 in a row!");
    sfText_setFont(title, titleFont);
    sfText_setCharacterSize(title, 50);
    sfText_setPosition(title, (sfVector2f){50, 20});

    // Create normal font and text style
    textFont = sfFont_createFromFile("Montserrat.ttf");
    if (!textFont) {
        puts("Unable to create normal font, aborting.");
        sfRenderWindow_destroy(window);
        sfText_destroy(title);
        sfFont_destroy(titleFont);
        free(startButtonText);
        return 1;
    }
    text = sfText_create();
    sfText_setFont(text, textFont);

    // Start of interactable button initialisations

    startButtonText = malloc(strlen("Start Game") + 1);
    strcpy(startButtonText, "Start Game");
    startButton = sfRectangleShape_create();
    quitButton = sfRectangleShape_create();

    infoLabelText = malloc(strlen("Player 1s Turn") + 1);
    strcpy(infoLabelText, "Player 1s Turn");

    // Start filling the circle array with circle objects which will then be used to create the board

    shapeFill();

    // Finish filling circle array

    // Main loop that keeps program alive
    while (sfRenderWindow_isOpen(window)) {
        // Start Event Checking
        while (sfRenderWindow_pollEvent(window, &event)) {  // having this as an event loop is better than an if statement as it improves performance
            if (event.type == sfEvtClosed) {
                puts("Closing render window.");
                sfRenderWindow_close(window);         // Close Window object
            } else if (event.type == sfEvtResized) {  // catch the resize events

                // update the view to the new size of the window
                sfFloatRect visibleArea = {0, 0, event.size.width, event.size.height};
                puts("resize");  // TODO: scale the contents of the window still looking into this but this seems a good resource: https://github.com/SFML/SFML/wiki/Source%3A-Letterbox-effect-using-a-view
                sfRenderWindow_setView(window, sfView_createFromRect(visibleArea));
            } else if (event.type == sfEvtMouseButtonPressed && event.mouseButton.button == sfMouseLeft)  // if the user clicks we need to send this click to a click event handler to see if they clicked on a circle or somewhere random
                userClick(event.mouseButton.x, event.mouseButton.y);
        }
        // End Event Checking

        sfRenderWindow_clear(window, sfColor_fromRGB(100, 100, 255));  // Set window to blue like in processing u gotta clear every time or else they get layered

        // Start rendering the circle grid here

        int count = 0;
        int x = 50;
        int y = 100;
        int circleNum = 0;

        for (size_t i = 0; i < 7; i++) {                                      // draw the grid of circles from left to right 1 row at a time 7 wide 6 tall
            sfCircleShape_setPosition(circle[i][count], (sfVector2f){x, y});  // in the first loop this set position of circle 1 to be 50 50 starting from the top left so 50 across and 50 down
            sfRenderWindow_drawCircleShape(window, circle[i][count], NULL);   // this then draws the circle with its positional data on the window object

            x += 100;                   // we then add 100 to x to move the next circle to be at position 150 50
            circleNum++;                // we increase the circle count so circle 2 in the array is selected to be drawn next
            if (i == 6 && count < 5) {  // when its looped 7 times we then reset the counter i to -1 and reset the x value to be 50 next we set y to be 150 this results in a new row of circles below being now created this is run 6 times starting at 0 then when count = 5 aka when its run 6 times the if statement doesnt run meaning i doesnt get reset and it finally leaves the for loop
                count++;
                i = -1;
                y += 100;
                x = 50;
            }
        }  // this happens 30 times a second im suprised it doesn't use more memory as im sure there is probably a better way to do this

        // End rendering the circle grid here

        // Start rendering the information card on the right

        sfRenderWindow_drawText(window, title, NULL);  // Title rendering
        // label("O", -1, -6, 25);                  // absolute top left
        label(infoLabelText, 875, 150, 25, sfWhite);  // we use 875 as it is the width of the button below divided by 2 add the x position of the rectangle which gives us the exact coordinates for the centre of the rectangle which means this label is in line with the label of the button below making things look neat

        button(startButton, 250, 50, 750, 200, sfBlue, startButtonText, 25, sfWhite);
        button(quitButton, 250, 50, 750, 650, sfBlue, "Quit Game", 25, sfWhite);

        // End rendering the information card on the right

        sfRenderWindow_display(window);  // Render window with changes made above
    }

    // Cleanup aka free designated memory

    for (size_t i = 0; i < 7; i++)
        for (size_t j = 0; j < 6; j++)
            sfCircleShape_destroy(circle[i][j]);

    sfRectangleShape_destroy(startButton);
    sfRectangleShape_destroy(quitButton);

    // sfMusic_destroy(music);
    sfText_destroy(text);
    sfFont_destroy(textFont);
    sfText_destroy(title);
    sfFont_destroy(titleFont);
    // sfSprite_destroy(sprite);
    // sfTexture_destroy(texture);
    sfRenderWindow_destroy(window);
    free(startButtonText);
    free(infoLabelText);
    return 0;
}

void button(sfRectangleShape *rectangle, int rectangleWidth, int rectangleHeight, int rectangleX, int rectangleY, sfColor rectangleColour, const char *userText, int fontSize, sfColor textColour) {  // a function that creates buttons with text inside of them this will be expanded in the future
    sfRectangleShape_setSize(rectangle, (sfVector2f){rectangleWidth, rectangleHeight});
    sfRectangleShape_setPosition(rectangle, (sfVector2f){rectangleX, rectangleY});
    sfRectangleShape_setFillColor(rectangle, rectangleColour);
    sfRenderWindow_drawRectangleShape(window, rectangle, NULL);

    // below is a fun little calculation that centres text inside of a rectangle shape
    // first we need to get the centre of the rectangle so we know where to place our text to do this we get the rectangles width and divide by 2 this gives us the centre pixel of the rectangle which we then add to its left (aka its x coordinate) in order to get
    // the global coordinate value of the centre of the rectangle next take off the width of the text /2 aka we remove the left half of the text from the x coordinate the calculation gave us so the centre of the text is in the exact same position as the centre of rectangle
    // then we do the same thing with the height except in this case we dont need to divide the text height by 2 due to how CSFML renders it then finally we have centered text YAY (yes its a little off but thats bc it does it based on the tallest character in the font family it could be fixed with https://en.sfml-dev.org/forums/index.php?topic=5790.0 but im not going to bother)

    sfText_setString(text, userText);
    sfText_setFillColor(text, textColour);
    sfText_setCharacterSize(text, fontSize);
    sfFloatRect textSize = sfText_getGlobalBounds(text);
    sfText_setPosition(text, (sfVector2f){(rectangleWidth / 2 + rectangleX) - textSize.width / 2, (rectangleHeight / 2 + rectangleY) - textSize.height});
    sfRenderWindow_drawText(window, text, NULL);
}

void label(const char *userText, int x, int y, int fontSize, sfColor textColour) {  // simple function to make drawing basic text easier by simply reusing the text variable and not creating a whole new one each time probably heavier on CPU processing but consumes less memory and the game isnt exactly complex so more cpu processing is better than more memory use
    sfText_setString(text, userText);
    sfText_setCharacterSize(text, fontSize);
    sfText_setFillColor(text, textColour);
    sfFloatRect textSize = sfText_getGlobalBounds(text);
    sfText_setPosition(text, (sfVector2f){x - textSize.width / 2, y});  // we do this to centre the text around the points given making it much easier to work out where to place labels
    sfRenderWindow_drawText(window, text, NULL);
}

void shapeFill() {
    for (size_t i = 0; i < 7; i++) {
        for (size_t j = 0; j < 6; j++) {
            circle[i][j] = sfCircleShape_create();
            sfCircleShape_setRadius(circle[i][j], 30.0);
            sfCircleShape_setFillColor(circle[i][j], sfWhite);
        }
    }
}

void userClick(int xPos, int yPos) {
    sfVector2i mousePixelClick;  // this creates a special vector which contains the pixel coordinates of our mouse click we do this here to prevent running it in the seperate functions later on
    mousePixelClick.x = xPos;
    mousePixelClick.y = yPos;
    sfVector2f clickPos = sfRenderWindow_mapPixelToCoords(window, mousePixelClick, sfRenderWindow_getView(window));  // this converts those pixel coordinates to the actual coordinates inside of the currently rendered window for example lets say your window is in the top left of your screen and you press the bottom right of the window this is going to translate that click from being in the top left area of your monitor to the actualy potition  inside the window which is the bottom right... i think

    if (clickCircle(clickPos)) {
        puts("User clicked a circle, checking for 4 in a row...");
        winCheck();
    } else if (clickRectangle(clickPos))
        puts("User clicked a rectangle");
    else
        puts("User clicked elsewhere");
}

bool clickCircle(sfVector2f clickPos) {  // im not sure if this is the best approach but we literlly just check the WHOLE array of cirlces to see if the user has selected any of them ;;
    bool userClickedCircle = false;
    if (gameRunning) {
        for (size_t i = 0; i < 7; i++) {  // loop all 42 circles checking the location of the click against the exact area the circle takes up
            for (size_t j = 0; j < 6; j++) {
                sfFloatRect circlePos = sfCircleShape_getGlobalBounds(circle[i][j]);  // get the area of pixels contained within the first circle
                if (sfFloatRect_contains(&circlePos, clickPos.x, clickPos.y)) {       // if the user clicked within the current circle
                    userClickedCircle = true;

                    sfColor circleColour = sfCircleShape_getFillColor(circle[i][j]);  // get the current circle colour
                    if (circleColour.b == 255) {                                      // if the current circle has a blue colour value of 255 it means it cannot be yellow or red so it must be white so this is a valid place to put in a new circle colour
                        int currentCircleNumber = i + (j * 7);
                        printf("%d", currentCircleNumber);
                        if (currentCircleNumber + 7 < 42) {                                               // if not on the bottom row we need to check if the circles below us have colours if not we obviously can't place something here
                            sfColor belowColour = sfCircleShape_getFillColor(circle[i][j + 1]);           // get the colour of the circle below the current one
                            if (belowColour.b != 255) {                                                   // as long as blue is not 255 it means we have either a yellow or red circle below
                                sfCircleShape_setFillColor(circle[i][j], user1sTurn ? sfRed : sfYellow);  // fill the circle the user clicked with red or yellow depending on the turn
                                user1sTurn = !user1sTurn;
                                infoLabelText = malloc(strlen(user1sTurn ? "Player 1s Turn" : "Player 2s Turn") + 1);
                                strcpy(infoLabelText, user1sTurn ? "Player 1s Turn" : "Player 2s Turn");
                            } else {
                                puts("There is no colour below this");
                            }
                        } else {  // if we're on the bottom row we can just set the colour as we know this is a valid move
                            sfCircleShape_setFillColor(circle[i][j], user1sTurn ? sfRed : sfYellow);
                            user1sTurn = !user1sTurn;
                            infoLabelText = malloc(strlen(user1sTurn ? "Player 1s Turn" : "Player 2s Turn") + 1);
                            strcpy(infoLabelText, user1sTurn ? "Player 1s Turn" : "Player 2s Turn");
                        }

                    } else {  // the current circle colour is not white
                        puts("There is already a colour here");
                    }
                    break;
                }
            }
        }
    }
    return userClickedCircle;
}

bool clickRectangle(sfVector2f clickPos) {
    sfFloatRect startButtonPos = sfRectangleShape_getGlobalBounds(startButton);
    sfFloatRect quitButtonPos = sfRectangleShape_getGlobalBounds(quitButton);
    if (sfFloatRect_contains(&startButtonPos, clickPos.x, clickPos.y)) {
        gameRunning = true;
        shapeFill();
        startButtonText = malloc(strlen("Game Running") + 1);
        strcpy(startButtonText, "Game Running");
        return true;
    } else if (sfFloatRect_contains(&quitButtonPos, clickPos.x, clickPos.y)) {
        puts("Closing render window.");
        sfRenderWindow_close(window);  // Close Window object
        return true;
    }
    return false;
}

void winCheck() {
    int playerColour = sfColor_toInteger(user1sTurn ? sfYellow : sfRed);  // this is set to the opposite as when a player finishes their move the user turn is updated but in this case we want to ignore that update

    // Horizontal Win Check
    for (int i = 6; i > 0; i--) {      // height
        for (int j = 7; j > 0; j--) {  // width
            if (sfColor_toInteger(sfCircleShape_getFillColor(circle[j][i])) == playerColour && sfColor_toInteger(sfCircleShape_getFillColor(circle[j + 1][i])) == playerColour && sfColor_toInteger(sfCircleShape_getFillColor(circle[j + 2][i])) == playerColour && sfColor_toInteger(sfCircleShape_getFillColor(circle[j + 3][i])) == playerColour) {
                gameOver();
            }
        }
    }

    // Vertical Win Check
    for (int i = 6; i > 0; i--) {      // height
        for (int j = 7; j > 0; j--) {  // width
            if (sfColor_toInteger(sfCircleShape_getFillColor(circle[j][i])) == playerColour && sfColor_toInteger(sfCircleShape_getFillColor(circle[j][i + 1])) == playerColour && sfColor_toInteger(sfCircleShape_getFillColor(circle[j][i + 2])) == playerColour && sfColor_toInteger(sfCircleShape_getFillColor(circle[j][i + 3])) == playerColour) {
                gameOver();
            }
        }
    }

    // Diagonal Win Check left to right
    for (int i = 6; i > 0; i--) {      // height
        for (int j = 7; j > 0; j--) {  // width
            if (sfColor_toInteger(sfCircleShape_getFillColor(circle[j][i])) == playerColour && sfColor_toInteger(sfCircleShape_getFillColor(circle[j - 1][i + 1])) == playerColour && sfColor_toInteger(sfCircleShape_getFillColor(circle[j - 2][i + 2])) == playerColour && sfColor_toInteger(sfCircleShape_getFillColor(circle[j - 3][i + 3])) == playerColour) {
                gameOver();
            }
        }
    }

    // Diagonal Win Check right to left
    for (int i = 6; i > 0; i--) {      // height
        for (int j = 7; j > 0; j--) {  // width
            if (sfColor_toInteger(sfCircleShape_getFillColor(circle[j][i])) == playerColour && sfColor_toInteger(sfCircleShape_getFillColor(circle[j - 1][i - 1])) == playerColour && sfColor_toInteger(sfCircleShape_getFillColor(circle[j - 2][i - 2])) == playerColour && sfColor_toInteger(sfCircleShape_getFillColor(circle[j - 3][i - 3])) == playerColour) {
                gameOver();
            }
        }
    }
}

void gameOver() {
    infoLabelText = malloc(strlen(!user1sTurn ? "Player 1 wins!" : "Player 2 wins!") + 1);
    strcpy(infoLabelText, !user1sTurn ? "Player 1 wins!" : "Player 2 wins!");
    startButtonText = malloc(strlen("Play Again?") + 1);
    strcpy(startButtonText, "Play Again?");
    gameRunning = false;
}