#include <stdio.h>
#include <SFML/Graphics.h>
#include <SFML/Window.h>
#include <SFML/System.h>

// Preprocess function types
void clickCircle(int, int);
/* This is the actual Render window its defined like this as it is a pointer to an sfRenderWindow that we will then interact with */
sfRenderWindow *window;
// Create circle array
sfCircleShape *circle[42];

int main(int argc, char const *argv[])
{
    /* This will be used in the main loop its used to keep track of when certain events take place and lets us do things like event listeners (kinda) */
    sfEvent event;
    /* Default bit colour depth at set resolution */
    sfVideoMode mode = {760, 650, sfVideoMode_getDesktopMode().bitsPerPixel};
    /* Start creating the window i could use sfVideoMode_getDesktopMode() instead of mode however this is like the max ur screen can do and idk why ud ever want this so */
    window = sfRenderWindow_create(mode, "4 in a row", sfResize | sfClose, NULL);

    if (!window)
    {
        puts("Unable to create window, aborting.");
        return 1;
    }

    /* Set frame rate to 30fps to reduce CPU usage */
    sfRenderWindow_setFramerateLimit(window, 30);

    /* Start outer Setup */

    for (size_t i = 0; i < 42; i++)
    {
        circle[i] = sfCircleShape_create();
        sfCircleShape_setRadius(circle[i], 30.0);
        sfCircleShape_setFillColor(circle[i], sfWhite);
    }

    /* End outer Setup */

    /* Main loop that keeps program alive */
    while (sfRenderWindow_isOpen(window))
    {
        /* Start Event Checking */
        while (sfRenderWindow_pollEvent(window, &event)) // having this as an event loop is better than an if statement as it improves performance
        {
            if (event.type == sfEvtClosed)
            {
                puts("Closing render window.");
                sfRenderWindow_close(window); /* Close Window object */
            }
            else if (event.type == sfEvtResized) // catch the resize events
            {
                // update the view to the new size of the window
                sfFloatRect visibleArea = {0, 0, event.size.width, event.size.height};
                sfRenderWindow_setView(window, sfView_createFromRect(visibleArea));
            }
            else if (event.type == sfEvtMouseButtonPressed && event.mouseButton.button == sfMouseLeft)
            {
                clickCircle(event.mouseButton.x, event.mouseButton.y);
            }
        }
        // End Event Checking

        sfRenderWindow_clear(window, sfColor_fromRGB(100, 100, 255)); /* Set window to white like in processing u gotta clear every time or else they get layered */

        /* Start Main program rendering code here */

        int count = 0;
        int x = 50;
        int y = 50;
        int circleNum = 0;

        for (size_t i = 0; i < 7; i++) // draw the grid
        {
            sfCircleShape_setPosition(circle[circleNum], (sfVector2f){x, y});
            sfRenderWindow_drawCircleShape(window, circle[circleNum], NULL);
            x += 100;
            circleNum++;
            if (i == 6 && count < 5)
            { // when its looped 7 times (starts at 0)
                count++;
                i = -1;
                y += 100;
                x = 50;
            }
        }

        /* End Main program rendering code here */

        sfRenderWindow_display(window); /* Render window with changes made above */
    }
    /* Cleanup aka free designated memory */
    for (size_t i = 0; i < 42; i++)
    {
        sfCircleShape_destroy(circle[i]);
    }
    sfRenderWindow_destroy(window);
    return 0;
}

void clickCircle(x, y)
{
    sfVector2i mouseDownPosition;
    mouseDownPosition.x = x;
    mouseDownPosition.y = y;
    sfVector2f clickPos = sfRenderWindow_mapPixelToCoords(window, mouseDownPosition, sfRenderWindow_getView(window));
    for (size_t i = 0; i < 42; i++)
    {
        sfFloatRect circlePos = sfCircleShape_getGlobalBounds(circle[i]);
        if (sfFloatRect_contains(&circlePos, clickPos.x, clickPos.y))
        {
            sfColor circleColour = sfCircleShape_getFillColor(circle[i]);
            if (circleColour.r == sfWhite.r && circleColour.g == sfWhite.g && circleColour.b == sfWhite.b && circleColour.a == sfWhite.a)
            {
                if (i + 7 < 42)
                {
                    sfColor belowColour = sfCircleShape_getFillColor(circle[i + 7]);
                    if (belowColour.b != 255) // as long as blue is not 255 it means we have either yellow or red below
                    {
                        sfCircleShape_setFillColor(circle[i], sfRed);
                    }
                    else
                    {
                        puts("There is no colour below this");
                    }
                }
                else
                { // bottom row
                    sfCircleShape_setFillColor(circle[i], sfRed);
                }
            }
            else
            {
                puts("There is already a colour here");
            }
            break;
        }
    }
}