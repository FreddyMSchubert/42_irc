NAME = ircserv

CXX = c++
CXXFLAGS = -g -std=c++17 -Wall -Wextra -Werror -fsanitize=address

# TODO: Add all source files
SRCS = $(shell find ./src -name "*.cpp")
HEADER = $(shell find ./inc -name "*.hpp")

OBJDIR = obj
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

run: all
	./$(NAME) 6667 password password

re: fclean all

ren: re
	./$(NAME) 6667 password password

party:
	curl parrot.live
pedro:
	curl pedro.linuxhat.net