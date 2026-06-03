#pragma once

#include "app/base_renderer.h"
#include "app/base_world.h"
#include "input/input_handler.h"

#include <memory>
#include <utility>

struct BaseScene {
  std::unique_ptr<BaseWorld> world;
  std::unique_ptr<BaseRenderer> renderer;
  InputHandler input;

  InputHandler getDefaultInput();

  template <typename W, typename R>
  BaseScene(std::in_place_type_t<W>, std::in_place_type_t<R>,
            InputHandler &&input)
      : world{std::make_unique<W>()},
        renderer{std::make_unique<R>(static_cast<W *>(world.get()))},
        input{getDefaultInput()} {
    addNewInputs(std::move(input));
  }

private:
  void addNewInputs(InputHandler &&extra);
};