#include <string>
#include <vector>

#include "Core.hpp"

namespace vp {
struct TaskImpl {
  std::string name;
  std::vector<Token *> inputs;
  std::vector<Token *> outputs;

  void *args;

  TaskImpl() = delete;
  TaskImpl(const TaskImpl &other) = delete;
  TaskImpl &operator=(const TaskImpl &other) = delete;

  TaskImpl(const char *str_name, uint32_t num_inputs, uint32_t num_outputs, void *p_args)
      : name(str_name), inputs(num_inputs), outputs(num_outputs), args(p_args) {}
};

Task::Task(const char *str_name, uint32_t num_inputs, uint32_t num_outputs, void *p_args)
    : p_impl(new TaskImpl(str_name, num_inputs, num_outputs, p_args)) {}

TaskExecStatus Task::Run() { return TaskExecStatus::TASK_EXEC_SUCCESS; }

TaskExecStatus Task::Execute() {
  auto const ret = Run();
  return ret;
}

bool Task::SetInput(Token *p_input, uint32_t num_input) {
  if (num_input < p_impl->inputs.size()) {
    p_impl->inputs[num_input] = p_input;
    return true;
  }

  return false;
}

void Task::ClearInputs() {
  for (auto i = 0U; i < GetNumInputs(); i++) {
    SetInput(nullptr, i);
  }
}

Token *Task::GetInput(uint32_t num_input) {
  if (num_input < p_impl->inputs.size()) {
    return p_impl->inputs[num_input];
  }

  return nullptr;
}

bool Task::SetOutput(Token *p_output, uint32_t num_output) {
  if (num_output < p_impl->outputs.size()) {
    p_impl->outputs[num_output] = p_output;
    return true;
  }
  return false;
}

void Task::ClearOutputs() {
  for (auto i = 0U; i < GetNumOutputs(); i++) {
    SetOutput(nullptr, i);
  }
}

Token *Task::GetOutput(uint32_t num_output) {
  if (num_output < p_impl->outputs.size()) {
    return p_impl->outputs[num_output];
  }

  return nullptr;
}

Task::~Task() { delete p_impl; }

size_t Task::GetNumOutputs() const { return p_impl->outputs.size(); }

size_t Task::GetNumInputs() const { return p_impl->inputs.size(); }
}