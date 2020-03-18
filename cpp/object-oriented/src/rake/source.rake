### constants
class Source
  OUT_DIR = 'output'
  OBJS_DIR = ".objects/"
  OUTPUT = "#{OUT_DIR}/cdp-cpp-example"

  SRC_FILES = FileList["src/**/*.cpp"]
  OBJ_FILES = SRC_FILES.ext('.o').map { |sf| File.join(".objects",sf) }

  FLAGS="-Isrc -Isrc/include -pthread -std=c++11"
end

### rules
#.c to .o
rule /.*\.o/ => proc { |t| t.ext('.cpp').sub(Source::OBJS_DIR, '') } do |t|
  puts "#{Display::BLUE}Compiling#{Display::BLANK} #{t.name} #{Display::BLUE}from#{Display::BLANK} #{t.source}"
  sh "mkdir -p #{File.dirname(t.name)}"
  sh "g++ -o #{t.name} -c #{t.sources.join(" ")} #{Source::FLAGS}"
end

#.o to binary
rule Source::OUTPUT => Source::OBJ_FILES do |t|
  puts Display::BLUE+"Finalizing binary"+Display::BLANK
  sh "mkdir -p #{Source::OUT_DIR}"
  sh "g++ -o #{Source::OUTPUT} #{t.sources.join(" ")} #{Source::FLAGS}"
end

### tasks
desc 'builds the source, disregarding if nothing has changed'
task source: Source::OUTPUT

desc 'cleans the output folder'
task :clean do
  sh "rm -rf #{Source::OUT_DIR} #{Source::OBJS_DIR}"
end

task default: :source
