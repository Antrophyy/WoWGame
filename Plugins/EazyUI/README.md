# EazyUI Plugin

A CommonUI-based framework plugin for screen/layer management and Enhanced Input-driven UI actions.

## Modules

- `EazyUI`: UI policy, layout, activatable widgets, modals, settings.
- `EazyUIInput`: Enhanced Input routing, action bindings, analog cursor, gamepad value forwarding.

## Runtime Flow

1. `UEazyGameUIManagerSubsystem` loads and owns the active `UEazyGameUIPolicy`.
2. `UEazyGameUIPolicy` creates per-local-player `UEazyPrimaryGameLayout` roots.
3. Gameplay/UI code pushes widgets into tagged layers via `UEazyPrimaryGameLayout`.
4. Widgets register input through `UEazyUIActionRouterBase::RegisterInputAction`.
5. Router keeps unique Enhanced Input bindings, then dispatches callbacks only to active bindings.

## Input Routing Notes

- Binding ownership lives in `FEazyActionBinding` static storage.
- `UEazyUIActionRouterBase` stores:
  - all handles (`AllActionBindings`)
  - active handles (`ActiveBindings`)
  - active handles indexed by `(InputAction, TriggerEvent)` (`ActiveBindingsByInputAction`)
- Dispatch path (`ProcessEnhancedInputAction`) uses the indexed cache for O(k) matching callbacks instead of scanning every active binding.
