#include "system.h"

#include <unistd.h>

#include <algorithm>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

// Retorna referência para a instância da CPU
Processor& System::Cpu() { return cpu_; }

// Retorna a lista de processos ordenada por uso de CPU
std::vector<Process>& System::Processes() {
  processes_.clear();
  std::vector<int> pids = LinuxParser::Pids();

  for (int pid : pids) {
    Process proc;
    proc.SetPid(pid);  // setter simples
    processes_.emplace_back(proc);
  }

  // Ordena por uso de CPU (decrescente)
  std::sort(processes_.begin(), processes_.end(),
            [](const Process& a, const Process& b) {
              return b < a;  // compara CPU de forma reversa
            });

  return processes_;
}

// Retorna a versão do kernel
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Retorna o nome do sistema operacional
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Retorna a utilização da memória
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Retorna o uptime do sistema
long System::UpTime() { return LinuxParser::UpTime(); }

// Retorna o número total de processos
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Retorna o número de processos em execução
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }
