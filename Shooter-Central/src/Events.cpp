/*#include "Events.h"

using namespace ShooterCentral;

Status pushEvent    (std::unique_ptr<ModelEvent> event){
    static std::queue<std::unique_ptr<ModelEvent>> eventQueue;

    eventQueue.push(std::move(event));
}
Status pushEvent    (std::unique_ptr<UIEvent> event);

void pollEvent      (std::unique_ptr<ModelEvent>& event);
void pollEvent      (std::unique_ptr<UI>& event);

*/

