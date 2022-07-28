#pragma once

#include <vector>
#include <functional>
#include <map>

#include <SDL2/SDL.h>

class InputHandler
{

private:

    static std::vector<char> keys; // Keys that are currently active
    static std::map<char, Uint64> keys_delay; // Frame delay before the keys will 
    // be allowed to be used again

public:

    InputHandler();
    ~InputHandler();

    void update();
    void set_delay(char c, int frames = 8);
    void add_key(char key);
    void remove_key(char key);

    std::vector<char> get_active_keys();

};


typedef std::vector<std::function<void(SDL_Event&)>> Callbacks;

class CallbackCollection
{
    /*
    CallbackCollection - Maps arbitrary int keys to callbacks to run.

    This class allows you to map an event to an arbitrary amount of callbacks, and vice versa.
    You can utilize these callbacks to create 'reactive' code,
    which is only ran when an event is encountered.

    We keep a map of keys to values.
    These keys can be any int, and should be extracted from the event by utilizing the 'extract_key()' method.
    From there, we run each function that is tied to the key, along with the GLOBAL callbacks,
    which are called on every event encountered.
    If a key is not found, then we run all callbacks under the UNKOWN key.

    Each callback will be given a reference to an event 
    */

    protected:

        std::map<int, Callbacks> callbacks;  // Maps of events to functions

    public:

        void handle_event(SDL_Event& event); // Sends an event through the handlers
        virtual int extract_key(SDL_Event& event) {return 0;}; // Extracts a valid key from the event
        void add_callback(int type, std::function<void(SDL_Event&)> func);  // Ties a callback to an event
        template<class T, class C>
        void add_callback_test(int type, T& thing, void (C::*)(SDL_Event&)); // Same as usual, but with class methods

};

class InputTemp: public CallbackCollection
{
    /*
    InputTemp - Temporary work in progress input system, may get a better name later...

    This input system operates under a callback approach.
    Components can attach callbacks that will be called once a certain event is encountered.
    Multiple callbacks can be tied to a certain event,
    and multiple keys can be attached to a callback.

    We also support callback ordering, which determines the order the callbacks are called in.
    We also support callback pairs(do we?) that will only trigger once a sequence of keys have been encountered,
    such as a key up and down event (This seems really ill defined, maybe this is unecessary?)

    Each callback will be given a reference to the given event.
    This event data can then be used to further process and deal with the event.

    We utilize some recursion to handle nested events.
    For example, keyboard events have data that can differentiae them from others,
    such as KEY_DOWN and KEY_UP, and the type of key that was pressed.
    We do this by creating callbacks that are apart of larger 
    */

    private:

        std::vector<SDL_Event> events;  // List of references to events that have been fired

    public:

        InputTemp();
        ~InputTemp();

        void get_event();  // Gets events from SDL and handles them
        int extract_key(SDL_Event& event) override;  // Implement the virtual method
};

class KeyCallbacks: public CallbackCollection
{
    /*
    KeyCallback - Ties callbacks to certain keys.

    We are designed to work with KeyboardEvents.
    Otherwise, we may run into issues.

    The key we extract from the event is 'event.key.keysym.sym',
    which should be an int that represents a certain key.
    */

   private:

        std::map<int, Callbacks> callbacks;  // Maps of events to functions

   public:

        KeyCallbacks();
        ~KeyCallbacks();

        int extract_key(SDL_Event& event) override;
};
