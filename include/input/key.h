#pragma once

#include <functional>

struct Key {
  using Callback = std::function<void(const float)>;
  // using Callback = void (*)(const double);
  // static constexpr Callback NONE = [](const double) {};

  static constexpr struct Hold {
  } HOLD{};
  static constexpr struct Just {
  } JUST{};

  bool on = false;
  bool justUsed = false;

  int mods;

  Callback justOn, holdOn, justOff;

  Key(const Callback justOn, const Callback holdOn, const Callback justOff,
      const int mods);
  Key(Hold, const Callback holdOn, const int mods = -1);
  Key(Just, const Callback justOn, const int mods = -1);

  void react(const int action, const int mods);
  void reset();

  void operator()(const float dt);
};