Import("env")

def event(source,target, env):
    print(f"xxx Called on {source} by {target}")

print("Current CLI targets", COMMAND_LINE_TARGETS)
print("Current Build targets", BUILD_TARGETS)



env.AddPreAction("*", event)
