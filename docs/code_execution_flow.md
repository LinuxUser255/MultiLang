# Code Execution Flow Chart

## When `main.c` is Run

```
┌─────────────────────────────────────────────────────────────┐
│                      START: main()                          │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│  STACK ALLOCATION: char name[100]                           │
│  • 100 bytes allocated on the stack                         │
│  • Automatic memory management                              │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│  CALL: ask_name(name, 100)                                  │
│  → Jump to get_input.c                                      │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
        ┌────────────────────────────────────┐
        │    get_input.c: ask_name()         │
        ├────────────────────────────────────┤
        │ 1. printf("Enter your name: ")     │
        │ 2. Check if size > INT_MAX         │
        │ 3. fgets(name, 100, stdin)         │
        │    • Wait for user input           │
        │    • Read up to 99 chars + '\0'    │
        │ 4. Remove newline character        │
        │    name[strcspn(name, "\n")] = '\0'│
        │ 5. printf("Hello, %s!\n", name)    │
        │ 6. Return (void)                   │
        └────────────────┬───────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│  RETURN to main()                                           │
│  • Stack variable 'name' now contains user input            │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│  HEAP ALLOCATION: char *heap_name                           │
│  CALL: ask_name_malloc(100)                                 │
│  → Jump to get_input_mem.c                                  │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
        ┌────────────────────────────────────────┐
        │  get_input_mem.c: ask_name_malloc()    │
        ├────────────────────────────────────────┤
        │ 1. malloc(100 * sizeof(char))          │
        │    • Allocate 100 bytes on heap        │
        │                                        │
        │ 2. Check if malloc failed              │
        │    ┌─────────────────────┐             │
        │    │ if (name == NULL)   │             │
        │    │  → fprintf(stderr)  │             │
        │    │  → return NULL      │             │
        │    └─────────────────────┘             │
        │                                        │
        │ 3. printf("Enter your name: ")         │
        │                                        │
        │ 4. Check if size > INT_MAX             │
        │    • Clamp to INT_MAX if needed        │
        │                                        │
        │ 5. fgets(name, 100, stdin)             │
        │    • Wait for user input               │
        │    • Read up to 99 chars + '\0'        │
        │                                        │
        │ 6. Check if fgets succeeded            │
        │    ┌─────────────────────┐             │
        │    │ if (fgets != NULL)  │             │
        │    │  → Remove newline   │             │
        │    │  → Print greeting   │             │
        │    │  → return name      │             │
        │    └─────────────────────┘             │
        │    ┌─────────────────────┐             │
        │    │ else                │             │
        │    │  → free(name)       │             │
        │    │  → return NULL      │             │
        │    └─────────────────────┘             │
        └────────────────┬───────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│  RETURN to main()                                           │
│  • heap_name now points to allocated memory with user input│
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│  NULL CHECK: if (heap_name != NULL)                         │
└────────────────────────┬────────────────────────────────────┘
                         │
                    ┌────┴────┐
                    │         │
                   YES       NO (skip to return)
                    │         │
                    ▼         │
┌─────────────────────────────────────────┐                   │
│  printf("Allocated memory: %s\n")       │                   │
│  • Display the heap-allocated name      │                   │
└────────────────────┬────────────────────┘                   │
                     │                                        │
                     ▼                                        │
┌─────────────────────────────────────────┐                   │
│  CALL: free_name(heap_name)             │                   │
│  → Jump to get_input_mem.c              │                   │
└────────────────────┬────────────────────┘                   │
                     │                                        │
                     ▼                                        │
        ┌────────────────────────────────┐                    │
        │  get_input_mem.c: free_name()  │                    │
        ├────────────────────────────────┤                    │
        │ 1. Check if (name != NULL)     │                    │
        │ 2. free(name)                  │                    │
        │    • Return memory to heap     │                    │
        │ 3. Return (void)               │                    │
        └────────────────┬───────────────┘                    │
                         │                                    │
                         ▼                                    │
┌─────────────────────────────────────────┐                   │
│  RETURN to main()                       │                   │
│  • heap_name memory is now freed        │                   │
└────────────────────┬────────────────────┘                   │
                     │                                        │
                     └────────────────────┬───────────────────┘
                                          │
                                          ▼
                         ┌────────────────────────────────────┐
                         │  return 0;                         │
                         │  • Stack variable 'name' destroyed │
                         │  • Program exits successfully      │
                         └────────────────┬───────────────────┘
                                          │
                                          ▼
                         ┌────────────────────────────────────┐
                         │           END PROGRAM              │
                         └────────────────────────────────────┘
```

## Summary of Execution Flow

1. **Stack Allocation** → `ask_name()` → User inputs name → Stored on stack
2. **Heap Allocation** → `ask_name_malloc()` → User inputs name → Stored on heap
3. **Display** → Print heap-allocated name
4. **Cleanup** → `free_name()` → Free heap memory
5. **Exit** → Stack automatically cleaned up, program terminates

## Memory State Timeline

| Step | Stack Memory | Heap Memory |
|------|--------------|-------------|
| Start | Empty | Empty |
| After line 7 | `name[100]` allocated | Empty |
| After `ask_name()` | `name` contains input | Empty |
| After `ask_name_malloc()` | `name` + `heap_name` pointer | 100 bytes allocated |
| After `free_name()` | `name` + `heap_name` pointer | **Freed** ✓ |
| End | **Auto-freed** ✓ | Empty |