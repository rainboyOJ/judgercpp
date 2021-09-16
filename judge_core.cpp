#include <iostream>
#include <thread>
#include <chrono>
#include <utility>
#include <string>
#include <fstream>
#include <unistd.h> 
#include <variant>
#include <vector>
#include <algorithm>
#include <iomanip>

#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <grp.h>
#include <signal.h>

#include "judger.h"

// ============================  args parse 
// [C++17 Command Line Parsing!](http://schneegans.github.io/tutorials/2019/08/06/commandline)
// fake operator
std::stringstream& operator>>( std::stringstream & os , std::vector<std::string> & __t){
    throw std::string(__FUNCTION__) + std::to_string(__LINE__) + "这个函数永远不应该被执行";
    return os;
}

class CommandLine {
 public:

  // These are the possible variables the options may point to. Bool and
  // std::string are handled in a special way, all other values are parsed
  // with a std::stringstream. This std::variant can be easily extended if
  // the stream operator>> is overloaded. If not, you have to add a special
  // case to the parse() method.
  typedef std::variant<int32_t*, 
                       uint32_t*,
                       uint64_t*,
                       double*, 
                       float*, 
                       bool*, 
                       std::string*,
                        std::vector<std::string>*
                           > Value;

  // The description is printed as part of the help message.
  explicit CommandLine(std::string description)
    : mDescription(std::move(description)) {
    }


  // Adds a possible option. A typical call would be like this:
  // bool printHelp = false;
  // cmd.addArgument({"--help", "-h"}, &printHelp, "Print this help message");
  // Then, after parse() has been called, printHelp will be true if the user
  // provided the flag.
  void addArgument(std::vector<std::string> const& flags, 
          Value const& value, std::string const& help) {
      mArguments.emplace_back(Argument{flags, value, help});
  }

  // Prints the description given to the constructor and the help
  // for each option.
  void printHelp(std::ostream& os = std::cout) const {
    // Print the general description.
    os << mDescription << std::endl;

    // Find the argument with the longest combined flag length (in order
    // to align the help messages).

    uint32_t maxFlagLength = 0;

    for (auto const& argument : mArguments) {
        uint32_t flagLength = 0;
        for (auto const& flag : argument.mFlags) {
            // Plus comma and space.
            flagLength += static_cast<uint32_t>(flag.size()) + 2;
        }

        maxFlagLength = std::max(maxFlagLength, flagLength);
    }

    // Now print each argument.
    for (auto const& argument : mArguments) {

        std::string flags;
        for (auto const& flag : argument.mFlags) {
            flags += flag + ", ";
        }

        // Remove last comma and space and add padding according to the
        // longest flags in order to align the help messages.
        std::stringstream sstr;
        sstr << std::left << std::setw(maxFlagLength) 
            << flags.substr(0, flags.size() - 2);

        // Print the help for each argument. This is a bit more involved
        // since we do line wrapping for long descriptions.
        size_t spacePos  = 0;
        size_t lineWidth = 0;
        while (spacePos != std::string::npos) {
            size_t nextspacePos = argument.mHelp.find_first_of(' ', spacePos + 1);
            sstr << argument.mHelp.substr(spacePos, nextspacePos - spacePos);
            lineWidth += nextspacePos - spacePos;
            spacePos = nextspacePos;

            if (lineWidth > 60) {
                os << sstr.str() << std::endl;
                sstr = std::stringstream();
                sstr << std::left << std::setw(maxFlagLength - 1) << " ";
                lineWidth = 0;
            }
        }
    }
}

  // The command line arguments are traversed from start to end. That means,
  // if an option is set multiple times, the last will be the one which is
  // finally used. This call will throw a std::runtime_error if a value is
  // missing for a given option. Unknown flags will cause a warning on
  // std::cerr.
  void parse(int argc, char* argv[]) const {

      // Skip the first argument (name of the program).
      int i = 1;
      while (i < argc) {

          // First we have to identify wether the value is separated by a space
          // or a '='.
          std::string flag(argv[i]);
          std::string value;
          bool        valueIsSeparate = false;

          // If there is an '=' in the flag, the part after the '=' is actually
          // the value.
          size_t equalPos = flag.find('=');
          if (equalPos != std::string::npos) {
              value = flag.substr(equalPos + 1);
              flag  = flag.substr(0, equalPos);
          }
          // Else the following argument is the value.
          else if (i + 1 < argc) {
              value           = argv[i + 1];
              valueIsSeparate = true;
          }

          // Search for an argument with the provided flag.
          bool foundArgument = false;

          for (auto const& argument : mArguments) {
              if (std::find(argument.mFlags.begin(), argument.mFlags.end(), flag) 
                      != std::end(argument.mFlags)) {

                  foundArgument = true;

                  // In the case of booleans, there must not be a value present.
                  // So if the value is neither 'true' nor 'false' it is considered
                  // to be the next argument.
                  if (std::holds_alternative<bool*>(argument.mValue)) {
                      if (!value.empty() && value != "true" && value != "false") {
                          valueIsSeparate = false;
                      }
                      *std::get<bool*>(argument.mValue) = (value != "false");
                  }
                  // In all other cases there must be a value.
                  else if (value.empty()) {
                      throw std::runtime_error(
                              "Failed to parse command line arguments: "
                              "Missing value for argument \"" + flag + "\"!");
                  }
                  // For a std::string, we take the entire value.
                  else if (std::holds_alternative<std::string*>(argument.mValue)) {
                      *std::get<std::string*>(argument.mValue) = value;
                  }
                  // For a std::vector<std::string>
                  else if ( std::holds_alternative<std::vector<std::string>*>(argument.mValue)){
                        (*std::get<std::vector<std::string>*>(argument.mValue) ).emplace_back(std::move(value));
                  }
                  // In all other cases we use a std::stringstream to
                  // convert the value.
                  else {
                      std::visit(
                              [&value](auto&& arg) {
                              std::stringstream sstr(value);
                              sstr >> *arg;
                              },
                              argument.mValue);
                  }

                  break;
              }
          }

          // Print a warning if there was an unknown argument.
          if (!foundArgument) {
              std::cerr << "Ignoring unknown command line argument \"" << flag
                  << "\"." << std::endl;
          }

          // Advance to the next flag.
          ++i;

          // If the value was separated, we have to advance our index once more.
          if (foundArgument && valueIsSeparate) {
              ++i;
          }
      }
  }

 private:
  struct Argument {
      std::vector<std::string> mFlags;
      Value                    mValue;
      std::string              mHelp;
  };

  std::string           mDescription;
  std::vector<Argument> mArguments;
};
// ============================  args parse  END


#define UNLIMITED 0

// 全局变量
//// == for arguments
CommandLine cmd("judger for noier and acmer.");

bool     help                    = false;
bool     show_version            = false;

uint32_t version = 1;

//// 程序比较运行需要的变量

struct config {
    uint32_t    max_cpu_time{UNLIMITED};
    uint32_t    max_real_time{UNLIMITED};
    uint32_t    max_process_number{UNLIMITED};
    bool        memory_limit_check_only{false};
    uint64_t    max_output_size{UNLIMITED};
    uint64_t    max_memory{UNLIMITED};
    uint64_t    max_stack{UNLIMITED};
    std::string exe_path{"1"};
    std::string input_path{"/dev/stdin"};
    std::string output_path{"/dev/stdout"};
    std::string error_path{"/dev/stderr"};
    std::vector<std::string> args{};
    std::vector<std::string> env{};
    std::string log_path{"judger_log.txt"};
    std::string seccomp_rule_name;
    uint32_t     uid{65534};
    uint32_t     gid{65534};
} CONFIG;


result RESULT;

// ==================== utils
template<char Delimiter = ' ',typename... Args>
void debug_out(std::ostream &os, Args&&... args){
    ( (os << args << Delimiter),... ) <<std::endl;
}

#ifdef DEBUG
//#define __print_result(node) debug_out<'\0'>(std::cout,#node ":","\t\t",RESULT.node)
#define __print_result(node) std::cout << std::setw(12) << #node ": " << RESULT.node <<'\n';

#else
#define __print_result(node) debug_out(std::cout,RESULT.node)
#endif

#define print_result()\
    __print_result(cpu_time);\
    __print_result(real_time);\
    __print_result(memory);\
    __print_result(signal);\
    __print_result(exit_code);\
    __print_result(error);\
    __print_result(result);

//TODO log
struct LOG {
    //LOG() = delete;
    //LOG(const char * file_name) 
        //: file_name{file_name},ofs{file_name}
    //{};
    ~LOG(){ ofs.close(); }
    void init(const char * file_name) {
        ofs = std::ofstream(file_name);
    }
    template<typename... Args>
    void write(Args&&... args){
        debug_out(ofs, std::forward<Args>(args)...);
    }
    std::ofstream ofs;
} __LOG__;

#define LOG_INIT(arg)   __LOG__.init(arg)
#define log_write( TAG, ...) __LOG__.write(TAG,"[at Function]:",__FUNCTION__,"[at LINE]:",__LINE__,';',__VA_ARGS__)
//TODO
#define log_error(...)     log_write("[ERROR]",__VA_ARGS__)
#define log_waring(...)    log_write("[WARNING]",__VA_ARGS__)
#define log_fatal(...)     log_write("[FATAL]",__VA_ARGS__)
#define log_info(...)      log_write("[INFO]",__VA_ARGS__)

#define ERROR_EXIT(error_code)\
    {\
        log_error("error_code:",error_code,"<->",#error_code);\
        _result.error = error_code; \
        return; \
    }
#define CHILD_ERROR_EXIT(error_code)\
    {\
        log_fatal("System_error:",#error_code);\
        __LOG__.~LOG();\
        raise(SIGUSR1);\
        exit(EXIT_FAILURE);\
    }


// ==================== utils end
// ==================== 子进程处理
//
template <class C, typename T>
T getPointerType(T C::*v);

template<typename CONFIG,typename Member,
    typename LIMIT_Type,
    typename pointTotype = decltype(getPointerType(std::declval<Member>()))
>
bool __do_serlimit(CONFIG& c,Member m,
        pointTotype v,LIMIT_Type limit){
    if(c.*m != UNLIMITED){
        struct rlimit LIMIT;
        LIMIT.rlim_cur = LIMIT.rlim_max = static_cast<rlim_t>(v);
        return setrlimit(limit, &LIMIT) == 0;
    }
    return true;
}

#define do_serlimit(Member,LIMIT_Type)\
    if( not __do_serlimit(_config,&config::Member,_config.Member,LIMIT_Type) )\
            CHILD_ERROR_EXIT(SETRLIMIT_FAILED);

#define do_serlimit_new_value(Member,LIMIT_Type,value)\
    if( not __do_serlimit(_config,&config::Member,value,LIMIT_Type) )\
            CHILD_ERROR_EXIT(SETRLIMIT_FAILED);



void child_process(config & _config){
    FILE *input_file = NULL, *output_file = NULL, *error_file = NULL;
    // 栈空间
    do_serlimit(max_stack,RLIMIT_STACK)

    // 内存
    if( not _config.memory_limit_check_only ){
        do_serlimit_new_value(max_memory,RLIMIT_AS,_config.max_memory*2)
    }

    //cpu time
    do_serlimit_new_value(max_cpu_time, RLIMIT_CPU,(_config.max_cpu_time+1000)/1000 )

    //
    do_serlimit(max_process_number, RLIMIT_NPROC)

    do_serlimit(max_output_size, RLIMIT_FSIZE)

    if(_config.input_path.length() != 0){
        input_file = fopen(_config.input_path.c_str(), "r");
        if (input_file == NULL) {
            CHILD_ERROR_EXIT(DUP2_FAILED);
        }

        if( dup2(fileno(input_file),fileno(stdin)) == -1){
            CHILD_ERROR_EXIT(DUP2_FAILED);
        }
    }

    if(_config.output_path.length() != 0){
        output_file = fopen(_config.output_path.c_str(), "w");
        if (output_file == NULL) {
            CHILD_ERROR_EXIT(DUP2_FAILED);
        }

        if( dup2(fileno(output_file),fileno(stdout)) == -1){
            CHILD_ERROR_EXIT(DUP2_FAILED);
        }
    }


    if (_config.error_path.length() != 0 ) {
        // if outfile and error_file is the same path, we use the same file pointer
        if (_config.output_path.length() != 0 &&_config.output_path == _config.error_path) {
            error_file = output_file;
        }
        else {
            error_file = fopen(_config.error_path.c_str(), "w");
            if (error_file == NULL) {
                CHILD_ERROR_EXIT(DUP2_FAILED);
            }
        }
        // redirect stderr -> file
        if (dup2(fileno(error_file), fileno(stderr)) == -1) {
            CHILD_ERROR_EXIT(DUP2_FAILED);
        }
    }

#ifndef LOCAL
    //set gid
    gid_t group_list[] = {_config.gid};
    if (_config.gid != -1 && (setgid(_config.gid) == -1 || setgroups(sizeof(group_list) / sizeof(gid_t), group_list) == -1)) {
        CHILD_ERROR_EXIT(SETUID_FAILED);
    }
#endif

#ifndef LOCAL
    //set uid
    if (_config.uid != -1 && setuid(_config.uid) == -1) {
        CHILD_ERROR_EXIT(SETUID_FAILED);
    }
#endif

    char * args[256]{NULL},* env[256]{NULL};
    for(int i = 0 ;i < 256 && i < _config.args.size(); ++i)
        args[i] = _config.args[i].data();
    for(int i = 0 ;i < 256 && i < _config.env.size(); ++i)
        env[i] = _config.env[i].data();

    execve(_config.exe_path.c_str(), args,env);
    CHILD_ERROR_EXIT(EXECVE_FAILED);

}

// ==================== Function
void run(config & _config, result &_result) {

#ifndef LOCAL
    // check whether current user is root
    uid_t uid = getuid();
    if (uid != 0) {
        ERROR_EXIT(ROOT_REQUIRED);
    }
#endif
        
    // TODO check arguments



  //std::chrono::time_point< std::chrono::system_clock > 
      auto time_start = std::chrono::system_clock::now();

    pid_t child_pid = fork();
    if( child_pid < 0 ){
        ERROR_EXIT(FORK_FAILED);
    }
    else if (child_pid == 0){
        // child_process
        child_process(_config);
    }
    else if ( child_pid > 0){
        int status;
        struct rusage resource_usage;
        //wait4 等到pid进程结束,并得到resource https://man7.org/linux/man-pages/man2/wait4.2.html
        // WSTOPPED 直到stop https://man7.org/linux/man-pages/man2/wait.2.html
        if( wait4(child_pid, &status, WSTOPPED, &resource_usage) == -1) {
            kill(child_pid,SIGKILL);
            ERROR_EXIT(WAIT_FAILED);
        }



        //std::chrono::time_point< std::chrono::system_clock > 
        auto time_end = std::chrono::system_clock::now();

        std::chrono::duration<double, std::milli>(time_start
                - time_end).count();
 

//WIFSIGNALED(wstatus) returns true if the child process was terminated by a signal.
              // WTERMSIG(wstatus)
              //    returns the number of the signal that caused the child
              //    process to terminate.  This macro should be employed only
              //    if WIFSIGNALED returned true.
        if (WIFSIGNALED(status) != 0) {
            _result.signal = WTERMSIG(status); //被哪个信号关闭的
        }

        if(_result.signal == SIGUSR1) {
            _result.result = SYSTEM_ERROR; // 为什么SIGUSR1 表示系统错误
        }
        else {
            _result.exit_code = WEXITSTATUS(status);
            _result.cpu_time = (int) (resource_usage.ru_utime.tv_sec * 1000 +
                                       resource_usage.ru_utime.tv_usec / 1000);
            _result.memory = resource_usage.ru_maxrss * 1024; // kb to bytes

            if (_result.exit_code != 0) {
                _result.result = RUNTIME_ERROR;
            }

            if (_result.signal == SIGSEGV) { // 段错误
                if (_config.max_memory != UNLIMITED && _result.memory > _config.max_memory) {
                    _result.result = MEMORY_LIMIT_EXCEEDED; //超内存
                }
                else {
                    _result.result = RUNTIME_ERROR; // 运行错误
                }
            }
            else {
                if (_result.signal != 0) {
                    _result.result = RUNTIME_ERROR;
                }
                if (_config.max_memory != UNLIMITED && _result.memory > _config.max_memory) {
                    _result.result = MEMORY_LIMIT_EXCEEDED;
                }
                if (_config.max_real_time != UNLIMITED && _result.real_time > _config.max_real_time) {
                    _result.result = REAL_TIME_LIMIT_EXCEEDED;
                }
                if (_config.max_cpu_time != UNLIMITED && _result.cpu_time > _config.max_cpu_time) {
                    _result.result = CPU_TIME_LIMIT_EXCEEDED;
                }
            }
        }
    }
}
// ==================== Function end



int main(int argc,char *argv[]){


    // ================= 命令行解析 功能
    // example :
    //  judger -h
    //  judger -t=1000 -e 1         设置时间为1000ms 执行程序 1
    //  judger --max_cpu_time=1000 -e 1
    cmd.addArgument({"-h", "--help"},    &help,    "输出帮助,然后退出。");
    cmd.addArgument({"-v", "--version"}, &show_version, "输出版本号,然后退出。");
    cmd.addArgument({"-t","--max_cpu_time"}, &CONFIG.max_cpu_time,  "程序的最大CPU运行时间 (ms),0 表示没有限制,默认为0。");
    cmd.addArgument({"-r","--max_real_time"}, &CONFIG.max_real_time,  "程序的最大实际时间(ms),0表示没有限制,默认为0。");
    cmd.addArgument({"-m","--max_memory"}, &CONFIG.max_memory,  "程序运行的内存限制(byte),默认512M。");
    cmd.addArgument({"-only","--memory_limit_check_only"}, &CONFIG.memory_limit_check_only,  "只检查内存使用，不加以限制，默认为False");
    cmd.addArgument({"-s","--max_stack"}, &CONFIG.max_stack,  "栈空间限制,默认没有限制");
    cmd.addArgument({"-p","--max_process_number"}, &CONFIG.max_process_number,  "最大进程数量，默认没有限制");
    cmd.addArgument({"-os","--max_output_size"}, &CONFIG.max_output_size,  "最大输出大小(byte)");

    cmd.addArgument({"-e","--exe_path"}, &CONFIG.exe_path, "要判断的程序的路径");
    cmd.addArgument({"-i","--input_path"}, &CONFIG.input_path,  "输入文件路径");
    cmd.addArgument({"-o","--output_path"}, &CONFIG.output_path,  "输出文件路径");
    cmd.addArgument({"-ep","--error_path"}, &CONFIG.error_path,  "错误输出路径");

    cmd.addArgument({"--args"}, &CONFIG.args,  "程序运行的参数表"); // TODO
    cmd.addArgument({"--env"}, &CONFIG.env,  "环境表");

    cmd.addArgument({"--log_path"}, &CONFIG.log_path,  "日志路径,默认 judge_log.txt");
    cmd.addArgument({"-rule","--seccomp_rule_name"}, &CONFIG.seccomp_rule_name,  "Seccomp Rule Name");

#ifndef LOCAL
    cmd.addArgument({"-u","--uid"}, &CONFIG.uid,  "UID (default 65534)");
    cmd.addArgument({"-g","--gid"}, &CONFIG.gid,  "GID (default 65534)");
#endif
    cmd.parse(argc, argv);

    if( help ){
        cmd.printHelp();
        return 0;
    }
    else if( show_version ){
        std::cout << "version: "  << version << std::endl;
        return 0;
    }

    LOG_INIT(CONFIG.log_path.c_str());

    
    #ifdef DEBUG
#define print_config(node) std::cout << std::setw(25) << #node ":" << '\t'<< CONFIG.node << '\n'
        print_config(max_cpu_time);
        print_config(max_real_time);
        print_config(max_process_number);
        print_config(memory_limit_check_only);
        print_config(max_output_size);
        print_config(max_memory);
        print_config(max_stack);
        print_config(exe_path);
        print_config(input_path);
        print_config(output_path);
        print_config(error_path);
        print_config(log_path);
        print_config(seccomp_rule_name);
        print_config(seccomp_rule_name);
        print_config(uid);
        print_config(gid);

        std::cout << "--args count " << CONFIG.args.size() << std::endl ;
        for (const auto& e : CONFIG.args) {
            std::cout << "     " ;
            std::cout << e << std::endl;
        }
        std::cout << "\n\n" ;

        std::cout << "--env count " << CONFIG.env.size() << std::endl ;
        for (const auto& e : CONFIG.env) {
            std::cout << "     " ;
            std::cout << e << std::endl;
        }
        std::cout << "\n\n" ;
    #endif
    // ================= 命令行解析 功能 结束

    run(CONFIG, RESULT);
    print_result();
    return 0;
}
