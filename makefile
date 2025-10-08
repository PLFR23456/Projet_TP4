# Nom de l'exécutable
EXEC_NAME = run

# Compilateur
CC = gcc

# Flags GCC standards
CFLAGS = -Wall -Wextra -pedantic -g -I$(INC_DIR) -MMD -MP

# Flags des bibliotheques standard et externe
LIBFLAG =
LDFLAG = 

# Dossiers
SRC_DIR = ./src
INC_DIR = ./inc
BIN_DIR = ./bin
BUILD_DIR = ./build

# Fichiers sources et objets
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

# Chemin de l'exécutable
EXEC = $(BIN_DIR)/$(EXEC_NAME)

# Règle par défaut
all: $(EXEC)

# Compilation de l'exécutable
$(EXEC): $(OBJS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)
#@echo "$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)"

# Compilation des fichiers objets
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Création des dossiers si nécessaire
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Nettoyage
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d

aclean: clean
	rm -f $(EXEC)

# Force rebuild
rebuild: aclean all

# Inclusion des dépendances auto-générées
-include $(DEPS)

# Déclaration des règles "virtuelles"
.PHONY: all clean aclean rebuild