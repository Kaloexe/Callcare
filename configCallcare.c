/* Configuration ports GPIO via console

#include "call.h"

void configPin()
{
        printf("Pin bouton (27) : ");
        scanf("%d", &buttonPin);
        
        printf("Pin led état (X) : ");
        scanf("%d", &ledPinGreenState);
        
        printf("Pin led appel (9) : ");
        scanf("%d", &ledPinGreenCall);
        
        printf("Pin led problème (21) : ");
        scanf("%d", &ledPinRedError);
}
*/
