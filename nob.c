#define NOB_IMPLEMENTATION
#include "nob.h"

typedef struct {
  Nob_String_View *items;
  size_t count;
  size_t capacity;
} PositionalArgs;

typedef struct {
  bool flag_run;
  bool flag_gdb_run;
  bool flag_help;
  PositionalArgs positional_args;
} Args;

Args parse_args(int argc, char** argv) {
  Args args = {0};

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--run") == 0) {
      args.flag_run = true;
    }
    else if (strcmp(argv[i], "--gdb_run") == 0) {
      args.flag_gdb_run = true;
    }
    else if (strcmp(argv[i], "--help") == 0) {
      args.flag_help = true;
    }
    else {
      nob_da_append(&args.positional_args, nob_sv_from_cstr(argv[i]));
    }
  }

  return args;
}

void print_help(const char* bin_path) {
  printf("Usage: %s [OPTION...] file args...\n", bin_path);
  printf("\n");
  printf("--run      after successful build, invokes the compiled binary\n");
  printf("--gdb_run  after successful build, invokes the compiled binary under headless gdb\n");
  printf("--help     prints this help menu\n");
}

void free_args(Args* args) { nob_da_free(args->positional_args); }


void cc_append_common_flags(Nob_Cmd* cmd) {
  nob_cmd_append(cmd, "-std=c++20");
  nob_cmd_append(cmd, "-lfmt", "-lpthread");
  nob_cmd_append(cmd, "-Wall", "-Wextra");
  nob_cmd_append(cmd, "-fsanitize=address");
}

void cc_append_common_includes(Nob_Cmd* cmd) {
  nob_cmd_append(cmd, "-I.");
}

bool cc_compile(const char* source) {
  Nob_Cmd cmd = {0};
  nob_cmd_append(
    &cmd,
    "g++",
    "-O3",
    "-ggdb"
  );

  cc_append_common_flags(&cmd);
  cc_append_common_includes(&cmd);

  nob_cmd_append(&cmd, "-o", "main");
  nob_cmd_append(&cmd, source);

  return nob_cmd_run(&cmd);
}

bool rs_compile(const char* source) {
  Nob_Cmd cmd = {0};
  nob_cmd_append(&cmd, "rustc", "-O", "-g");
  nob_cmd_append(&cmd, "-o", "main");
  nob_cmd_append(&cmd, source);
  return nob_cmd_run(&cmd);
}

bool gdb_run(const char* binary_path, const PositionalArgs args) {
  Nob_Cmd cmd = {0};

  nob_cmd_append(&cmd, "gdb");
  nob_cmd_append(&cmd, "--quiet");
  nob_cmd_append(&cmd, "--batch");
  nob_cmd_append(&cmd, "-ex", "set debuginfod enabled off");
  nob_cmd_append(&cmd, "-ex", "set print thread-events off");
  nob_cmd_append(&cmd, "--ex", "run");
  nob_cmd_append(&cmd, "--ex", "bt full");
  nob_cmd_append(&cmd, "--ex", "exit");
  nob_cmd_append(&cmd, "--args", binary_path);

  for (size_t i = 1; i < args.count; i++) { nob_cmd_append(&cmd, args.items[i].data); }

  return nob_cmd_run(&cmd);
}

bool run(const char* binary_path, const PositionalArgs args) {
  Nob_Cmd cmd = {0};

  nob_cmd_append(&cmd, binary_path);
  for (size_t i = 0; i < args.count; i++) { nob_cmd_append(&cmd, args.items[i].data); }

  return nob_cmd_run(&cmd);
}

Nob_String_View get_filename_ext(const char* filename) {
  ssize_t j = -1;
  const ssize_t filename_len = strlen(filename);
  Nob_String_View sv = {
    .count = 0,
    .data = filename,
  };

  for (ssize_t i = filename_len - 1; i >= 0; i--) {
    if (filename[i] == '.') {
      j = i + 1;
      break;
    }
  }

  if (j < 0 || j >= filename_len) {
    return sv;
  }

  sv.count = (size_t)filename_len - j;
  sv.data = filename + j;
  return sv;
}

bool is_rust_file(const char* filename) {
  Nob_String_View sv = get_filename_ext(filename);

  if (sv.count < 2) { return false; }
  return strncmp(sv.data, "rs", 2) == 0;
}


Nob_String_Builder rust_project_json_buffer = {0};
bool walk_dir_populate_rust_crates_for_project_json(Nob_Walk_Entry entry) {
  if (strncmp(entry.path, "./.git", 6) == 0) { *entry.action = NOB_WALK_SKIP; }

  if (is_rust_file(entry.path)) {
    // Check if there was an entry prior, so we can add a comma.
    if (rust_project_json_buffer.items[rust_project_json_buffer.count - 1] == '}') {
      nob_sb_appendf(&rust_project_json_buffer, ",\n");
    }

    nob_sb_appendf(&rust_project_json_buffer, "    { \"root_module\": \"%s\", \"edition\": \"2021\", \"deps\": [] }", entry.path);
  }

  return true;
}

bool get_rust_sysroot(Nob_String_Builder *sb) {
  const char* tmp_stdout_filepath = "/tmp/_rustc_sysroot_tmp";
  Nob_Cmd cmd = {0};

  nob_cmd_append(&cmd, "rustc", "--print", "sysroot");

  if (!nob_cmd_run(&cmd, .stdout_path = tmp_stdout_filepath)) {
    nob_log(NOB_ERROR, "Failed to get rust systroot");
    return false;
  }

  if (!nob_read_entire_file(tmp_stdout_filepath, sb)) {
    return false;
  }

  return true;
}


bool generate_rust_project_json() {
  Nob_String_Builder sysroot_sb = {0};
  if (!get_rust_sysroot(&sysroot_sb)) {
    nob_sb_free(sysroot_sb);
    nob_log(NOB_ERROR, "Failed get rust sysroot path");
    return 1;
  }

  sysroot_sb.count--;
  nob_sb_appendf(&sysroot_sb, "/lib/rustlib/src/rust/library ");
  sysroot_sb.items[sysroot_sb.count-1] = '\0';


  nob_sb_appendf(&rust_project_json_buffer, "{\n");

  nob_sb_appendf(&rust_project_json_buffer, "  \"sysroot_src\": \"%s\",\n", sysroot_sb.items);
  nob_sb_free(sysroot_sb);

  nob_sb_appendf(&rust_project_json_buffer, "  \"crates\": [\n");
  if (!nob_walk_dir(".", walk_dir_populate_rust_crates_for_project_json)) { return false; }
  nob_sb_appendf(&rust_project_json_buffer, "\n  ],\n");

  nob_sb_appendf(&rust_project_json_buffer, "  \"fallback_include\": [\"**/*.rs\"],\n");
  nob_sb_appendf(&rust_project_json_buffer, "  \"procMacro\": { \"enable\": false },\n");
  nob_sb_appendf(&rust_project_json_buffer, "  \"workspace\": { \"root\": \".\" }\n");
  nob_sb_appendf(&rust_project_json_buffer, "}");

  if (!nob_write_entire_file("rust-project.json", rust_project_json_buffer.items, rust_project_json_buffer.count)) {
    nob_log(NOB_ERROR, "Failed to write to test file!");
    return false;
  }
  return true;
}


int main(int argc, char* argv[]) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  if (argc < 2) {
    nob_log(NOB_ERROR, "Expected path to source file");
    return 1;
  }

  Args args = parse_args(argc, argv);

  if (args.flag_help) {
    print_help(argv[0]);
    free_args(&args);
    return 0;
  }

  if (args.positional_args.count == 0) {
    nob_log(NOB_ERROR, "No positional arguments passed in");
    free_args(&args);
    return 1;
  }

  const char* src_filename = args.positional_args.items[0].data;

  if (is_rust_file(src_filename)) {
    if (!generate_rust_project_json()) {
      nob_log(NOB_ERROR, "Failed to generate rust project json file");
      free_args(&args);
      return 1;
    }

    if (!rs_compile(src_filename)) {
      nob_log(NOB_ERROR, "Failed to compile '%s'", src_filename);
      free_args(&args);
      return 1;
    }
  }

  else if (!cc_compile(src_filename)) {
    nob_log(NOB_ERROR, "Failed to compile '%s'", src_filename);
    free_args(&args);
    return 1;
  }


  if (args.flag_gdb_run) {
    if (!gdb_run("./main", args.positional_args)) {
      free_args(&args);
      return 1;
    }
  }
  else if (args.flag_run) {
    if (!run("./main", args.positional_args)) {
      free_args(&args);
      return 1;
    }
  }


  free_args(&args);
  return 0;
}
