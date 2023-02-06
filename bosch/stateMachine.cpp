#include <iostream>
#include <string>

class State {
 public:
  virtual void HandleInput() = 0;
  virtual std::string GetName() = 0;
};

class StateA : public State {
 public:
  void HandleInput() override {
    std::cout << "Handling input in StateA" << std::endl;
  }
  std::string GetName() override {
    return "StateA";
  }
};

class StateB : public State {
 public:
  void HandleInput() override {
    std::cout << "Handling input in StateB" << std::endl;
  }
  std::string GetName() override {
    return "StateB";
  }
};

class StateMachine {
 public:
  StateMachine() : current_state_(nullptr), transition_event_(false) {}

  void SetState(State* state) {
    current_state_ = state;
  }

  void HandleInput() {
    current_state_->HandleInput();
  }

  void CheckForEvent() {
    if (transition_event_) {
      transition_event_ = false;
      SetState(new_state_);
    }
  }

  std::string GetCurrentStateName() {
    return current_state_->GetName();
  }

  void TriggerTransitionEvent(State* new_state) {
    transition_event_ = true;
    new_state_ = new_state;
  }

 private:
  State* current_state_;
  bool transition_event_;
  State* new_state_;
};

int main() {
    StateMachine state_machine;
    StateA state_a;
    StateB state_b;

    state_machine.SetState(&state_a);
    std::cout << "Current state: " << state_machine.GetCurrentStateName() << std::endl;
    state_machine.HandleInput();

    state_machine.TriggerTransitionEvent(&state_b);
    state_machine.CheckForEvent();
    std::cout << "Current state: " << state_machine.GetCurrentStateName() << std::endl;
    state_machine.HandleInput();

    return 0;
}