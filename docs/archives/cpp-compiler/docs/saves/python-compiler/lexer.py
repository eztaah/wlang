def lexer(code):
    tokens = []
    i = 0
    while i < len(code):
        # gestion des nombres
        if code[i].isdigit():
            num = ''
            while i < len(code) and code[i].isdigit():
                num += code[i]
                i += 1
            tokens.append(('NUMBER', int(num)))

        # gestion des opérateurs
        elif code[i] == '+':
            tokens.append(('PLUS', None))
            i += 1
        elif code[i] == '*':
            tokens.append(('TIMES', None))
            i += 1
        elif code[i] == '=':
            if i+1 < len(code) and code[i+1] == '=':
                tokens.append(('EQUALS_EQUALS', None))
                i += 2
            else:
                tokens.append(('EQUALS', None))
                i += 1
        elif code[i] == ';':
            tokens.append(('SEMICOLON', None))
            i += 1

        # Gestion des parenthèses et accolades
        elif code[i] == '(':
            tokens.append(('LPAREN', None))
            i += 1
        elif code[i] == ')':
            tokens.append(('RPAREN', None))
            i += 1
        elif code[i] == '{':
            tokens.append(('LBRACE', None))
            i += 1
        elif code[i] == '}':
            tokens.append(('RBRACE', None))
            i += 1
        
        # Gestion des variables et des mots-clés
        elif code[i].isalpha():
            ident = ''
            while i < len(code) and (code[i].isalpha() or code[i].isdigit() or code[i] == '_'):
                ident += code[i]
                i += 1
            # Gestion des mots-clés
            if ident == "if":
                tokens.append(('IF', None))
            else:
                tokens.append(('IDENTIFIER', ident))

        # on ignore des espaces, tab, nouvelle lignes
        elif code[i] == ' ' or code[i] == '\n' or code[i] == '\t':
            i += 1

        # erreur sinon
        else:
            raise ValueError(f"Unknown character: {code[i]}")
    
    # ajout caractere de fin
    tokens.append(('EOF', None))

    return tokens




def pretty_print(tokens):
    print("[")
    for token in tokens:
        token_type, token_value = token
        # Ajoute un espace après le mot-clé 'SEMICOLON' pour une meilleure lisibilité
        if token_type == 'SEMICOLON':
            print(f"    ('{token_type}', {token_value}), \n")
        else:
            print(f"    ('{token_type}', {token_value}),")
    print("]")