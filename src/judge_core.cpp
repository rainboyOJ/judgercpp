#include <thread>
#include <string>

#include "common.h"

CommandLine cmd("judger for noier and acmer.");

bool     help;
bool     memory_limit_check_only;

uint32_t version;
uint32_t max_cpu_time;
uint32_t max_real_time;
uint32_t max_memory;
uint32_t max_stack;
uint32_t max_process_number;
uint32_t max_output_size;

std::string exe_path;
std::string input_path;
std::string output_path;
std::string error_path;

std::string args;
std::string env;

std::string log_path;
std::string seccomp_rule_name;

int32_t uid;
int32_t gid;




int main(int argc,char *argv[]){


cmd.addArgument({"-h", "--help"},    &help,    "Display This Help And Exit");
cmd.addArgument({"-v", "--version"}, &version, "Display Version Info And Exit");
cmd.addArgument({"-c","--max_cpu_time"}, &max_cpu_time,  "Max CPU Time (ms)");
cmd.addArgument({"-t","--max_real_time"}, &max_real_time,  "Max Real Time (ms)");
cmd.addArgument({"-m","--max_memory"}, &max_memory,  "Max Memory (byte)");
cmd.addArgument({"-only","--memory_limit_check_only"}, &memory_limit_check_only,  "only check memory usage, do not setrlimit (default False)");
cmd.addArgument({"-s","--max_stack"}, &max_stack,  "Max Stack (byte, default 16M)");
cmd.addArgument({"-p","--max_process_number"}, &max_process_number,  "Max Process Number");
cmd.addArgument({"-os","--max_output_size"}, &max_output_size,  "Max Output Size (byte)");

cmd.addArgument({"-e","exe_path"}, &exe_path, "Exe Path");
cmd.addArgument({"-i","input_path"}, &input_path,  "Input Path");
cmd.addArgument({"-o","output_path"}, &output_path,  "Output Path");
cmd.addArgument({"-ep","error_path"}, &error_path,  "Error Path");

cmd.addArgument({"-","--args"}, &args,  "Arg");
cmd.addArgument({"-","--env"}, &env,  "Env");

cmd.addArgument({"-","--log_path"}, &log_path,  "Log Path");
cmd.addArgument({"-","--seccomp_rule_name"}, &seccomp_rule_name,  "Seccomp Rule Name");

cmd.addArgument({"-","--uid"}, &uid,  "UID (default 65534)");
cmd.addArgument({"-","--gid"}, &gid,  "GID (default 65534)");
    return 0;
}
