NAME = bot_chatgpt

CXX = c++
CXXFLAGS = -g -std=c++17 -Wall -Wextra -Werror

TEMPLATE_DIR = ../../bot_template
TEMPLATE_SRCS = $(TEMPLATE_DIR)/Bot.cpp $(TEMPLATE_DIR)/Socket.cpp
SRCS = bot_chatgpt.cpp

LIBS = -lcurl

OBJDIR = obj
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
TEMPLATE_OBJS = $(TEMPLATE_SRCS:$(TEMPLATE_DIR)/%.cpp=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS) $(TEMPLATE_OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS) $(TEMPLATE_OBJS) $(LIBS)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(TEMPLATE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

run: all
	./$(NAME)

.PHONY: all clean fclean re run