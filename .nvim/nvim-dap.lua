-- this will be used to attach the debugger to the running c++ program
-- and to set breakpoints and stuff using the nvim-dap plugin
-- and the c++ debugger gdb

local dap = require('dap')

dap.adapters.cppdbg = {
  type = 'executable',
  command = '/usr/bin/lldb-vscode', -- adjust as needed
  name = "lldb"
}

dap.configurations.cpp = {
  {
    name = "launch bunny",
    type = "lldb",
    request = "launch",
    program = "run_bunny_run",
    cwd = "${workspaceFolder}",
    stopOnEntry = true,
    args = {},
  },
}
