losResult fileRead(const std::string path, char **buf, size_t *buf_size) noexcept
{
    losFileHandle handle;
    losFileOpenInfo file;
    file.fileBits = LOS_FILE_BIT_READ;
    file.path = path.c_str();
    file.path_size = path.size();
    losResult res;
    if ((res = losOpenFile(&handle, file)) != LOS_SUCCESS)
        return res;
    if ((res = losReadFile(handle, (void **)buf, buf_size)) != LOS_SUCCESS)
        return res;
    if ((res = losCloseFile(handle)) != LOS_SUCCESS)
        return res;
    return LOS_SUCCESS;
}

const inline std::string createP(std::string path) noexcept
{
    return std::string("$[asset_base]/") += path += ".lls";
}

void Parser::parse(const std::string &&source, bool debug) noexcept
{
    if (debug)
    {
        puts(std::string("\n\x1B[32mSrc:\x1B[33m").c_str());
        puts(colourSrcText(source).c_str());
        puts(std::string("\x1B[32mSrc End:\x1B[33m\n").c_str());
    }
    lexer_index = 0;
    try
    {
        std::vector<lexToken> lexer;
        {
            size_t line = 0;
            auto words_begin = std::sregex_token_iterator(source.begin(), source.end(), regex);
            auto words_end = std::sregex_token_iterator();
            std::vector<std::string> split;
            split.reserve(std::distance(words_begin, words_end));
            std::move(words_begin, words_end, std::back_inserter(split));
            lexer.reserve(split.size());
            size_t i = 1;
            const size_t size = split.size() - 1;
            bool skip = false;
            for (const auto &word : split)
            {
                if (skip)
                {
                    skip = false;
                    const size_t n = ++i;
                    i = std::min<size_t>(size, n);
                    continue;
                }
                auto o = reject.find(std::hash<std::string>{}(word));
                if (o != NULL)
                {
                    if (*o)
                        line++;
                    const size_t n = ++i;
                    i = std::min<size_t>(size, n);
                    continue;
                }
                lexToken token;
                auto t = lookup.find(std::hash<std::string>{}(word));
                if (t == NULL)
                {
                    token.token = LexToken::IDENTIFIER;
                    token.str_token = word;
                }
                else
                {
                    if (i < split.size())
                    {
                        auto k = lookup.find(std::hash<std::string>{}(split[i]));
                        if (k == NULL)
                            token.token = *t;
                        else
                        {
                            if ((*k) == LexToken::L_ARROW && (*t) == LexToken::SUB)
                            {
                                token.token = LexToken::S_ARROW;
                                skip = true;
                            }
                            else
                                token.token = *t;
                        }
                    }
                }
                token.line = line;
                const size_t n = ++i;
                i = std::min<size_t>(size, n);
                if (lexer.size() < i)
                    lexer.emplace_back(token);
            }
            lexer.shrink_to_fit();
        }
        // end lexer here
        //------------------------------------------------------------------------------------
        // now we parse
        if (lexer[getIndexGuard(lexer, true)].token != LexToken::MODULE_NAME)
        {
            error(lexer[getIndexGuard(lexer, true)], "the source file dose not start with module keyword");
            return;
        }

        std::string module_name = lexer[getIndexGuard(lexer, true)].str_token;
        last_modules.push_back(module_name);

        for (; lexer_index < lexer.size();)
        {
            if (lexer[getIndexGuard(lexer)].token == LexToken::IMPORT)
            {
                (void)getIndexGuard(lexer, true);
                std::string name = "";
                while (true)
                {
                    if (!isValidName(lexer[getIndexGuard(lexer)]))
                        error(lexer[getIndexGuard(lexer, true)], "this is not a valid import name");
                    else
                        name += lexer[getIndexGuard(lexer, true)].str_token;

                    if (lexer[getIndexGuard(lexer)].token != LexToken::DOT)
                        break;
                    (void)getIndexGuard(lexer, true);
                    name += "/";
                }

                if (std::all_of(last_modules.begin(), last_modules.end(),
                                [&name](std::string mod) { return mod.compare(name) != 0; }))
                {
                    warn(lexer[getIndexGuard(lexer)],
                         std::string(std::string(name) += " is already loaded so will be ignored").c_str());
                    continue;
                }

                char *src;
                size_t src_size = 0;
                if (fileRead(createP(name), &src, &src_size) != LOS_SUCCESS)
                    error(lexer[getIndexGuard(lexer)], "could not find or read import");
                else
                {
                    uint64_t index = getIndexGuard(lexer);
                    if (debug)
                    {
                        printf("\x1B[94mParsing Dependency Module: \x1B[33m %s \x1B[00m -> \x1B[94m For "
                               "Module:\x1B[33m %s "
                               "\x1B[00m\n",
                               name.c_str(), module_name.c_str());
                    }
                    parse(std::string(src, 0, src_size), debug);
                    lexer_index = index;
                }
            }
            else
                break;
        }
        if (hasErrors())
            return;
        std::vector<lexToken> module_data;
        for (uint64_t i = lexer_index; i < lexer.size(); i++)
            module_data.emplace_back(lexer.at(i));
        if (debug)
            printf("\x1B[94mParsing Module: \x1B[33m %s \x1B[00m\n", module_name.c_str());
        root->modules.push_back(parseModule(std::move(module_data), module_name));
        if (debug)
            puts("\x1B[94mParsed\x1B[00m\n");
    }
    catch (const std::exception &e)
    {
        gf_error_stack->push(e.what());
    }
}