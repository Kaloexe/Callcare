#include "call.h"


void configPin()
{
        printf("Pin bouton : ");
        scanf("%d", &buttonPin);
        
        printf("Pin led état : ");
        scanf("%d", &ledPinGreenState);
        
        printf("Pin led appel : ");
        scanf("%d", &ledPinGreenCall);
        
        printf("Pin led problème : ");
        scanf("%d", &ledPinRedError);
}
