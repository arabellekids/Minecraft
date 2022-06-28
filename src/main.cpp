#include "app/app.h"

int main()
{
    App app(640, 480, false);

    if(!app.IsRunning()) { return -1; }
    app.Run();
    
    return 0;
}