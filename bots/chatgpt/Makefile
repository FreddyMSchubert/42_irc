NAME = chatgpt

CXX = c++
CXXFLAGS = -g -std=c++17

LIBS = -lcurl

SRCS = $(shell find . -name "*.cpp")
HEADER = $(shell find . -name "*.hpp")

OBJDIR = obj
OBJS = $(SRCS:./%.cpp=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS) $(LIBS)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

run: all
	./$(NAME) 127.0.0.1 6667 chatgptBOT chatgpt password
