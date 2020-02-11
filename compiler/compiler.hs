import Text.ParserCombinators.Parsec
import Debug.Trace

data Command
    = Label String
    | Inst Instruction 
    deriving ()

instance Show Command where
    show (Label s) = "Label (" ++ show s ++ ")"
    show (Inst i) = "Inst (" ++ show i ++ ")"

data Instruction
    = Nop 
    | Lda Address
    | Add Address
    | Sub Address
    | Sta Address
    | Ldi Immidiate
    | Jmp Address
    | Out 
    | Hlt 
    | PseudoByte Immidiate
    deriving (Show)

data Immidiate = Immidiate Int deriving (Show)

data Address
    = AddrRef String
    | AddrImm Immidiate
    deriving (Show)


p_file = 
    do
        cmds <- (p_spaces >> p_line >>= \p -> return p) `sepBy` eol
        eof
        return (foldr (++) [] cmds)

p_space = (char ' ' <|> char '\t') 
p_spaces = many p_space
p_spaces1 = many1 p_space
-- p_linespace = char ' ' <|> char '\t'
-- p_linespaces = many1 p_linespace

p_line = do 
            cmds <- ( try ( p_label >>= \l -> p_instruction >>= \i -> return [l,i] ) <|> 
                      try ( p_instruction >>= \i -> return [i] ) <|> 
                      try ( p_label >>= \l -> return [l] ) <|> 
                          ( p_spaces >> return []) )
            trace ("LINE: "++show cmds) p_spaces
            return cmds

p_label = do 
    name <- (many1 letter)
    char ':' <?> "end of label"
    p_spaces
    -- trace ("LABEL: "++name) p_spaces 
    return (Label name)

p_string s = do
            str <- string s
            p_spaces
            return str


p_string1 s = do
    str <- string s
    p_spaces1
    return str

p_instruction = do
            i <- ( try (p_string "nop"  >> return (Inst $ Nop ) ) <|> 
                   try (p_string "lda"  >> p_addr >>= \a -> return (Inst $ Lda a ) ) <|> 
                   try (p_string "add"  >> p_addr >>= \a -> return (Inst $ Add a ) ) <|> 
                   try (p_string "sub"  >> p_addr >>= \a -> return (Inst $ Sub a ) ) <|> 
                   try (p_string "sta"  >> p_addr >>= \a -> return (Inst $ Sta a ) ) <|> 
                   try (p_string "ldi"  >> p_imm  >>= \i -> return (Inst $ Ldi i ) ) <|> 
                   try (p_string "jmp"  >> p_addr >>= \a -> return (Inst $ Jmp a ) ) <|> 
                   try (p_string "out"  >> return (Inst $ Out ) ) <|> 
                   try (p_string "hlt"  >> return (Inst $ Hlt ) ) <|>
                   try (p_string ".byte" >> p_imm  >>= \i -> return (Inst $ PseudoByte i ) )
                )
            p_spaces
            -- trace ("INSTR: "++(show i)) p_spaces
            return i

p_addr = try (p_ref >>= \r -> return (r) ) <|>
             (p_imm >>= \i -> return (AddrImm i) ) <?> "address"

p_ref = do 
    name <- (many1 letter)
    p_spaces
    -- trace ("LABEL: "++name) p_spaces 
    return (AddrRef name)

p_imm = (many digit >>= \c -> return(Immidiate $ (read c :: Int)))

eol =   try (string "\n\r")
    <|> try (string "\r\n")
    <|> try (string "\n")
    <|> try (string "\r")
    <|> (p_spaces >> char ';' >> many (noneOf "\r\n") >> eol)
    <?> "end of line"


parseEaterASM :: String -> Either ParseError [Command]
parseEaterASM input = parse p_file "(unknown)" input

---------------
-- Assmebler --
---------------

assemble :: [Command] -> String
assemble cmds = do
    
    let labelmap = trace ("LINE: "++(show $ makeLabelMap cmds))
    return 'a'


makeLabelMap :: [Command] -> [(String, Int)]
makeLabelMap cmds = makeLabelMapAux cmds 0

makeLabelMapAux :: [Command] -> Int -> [(String, Int)]
makeLabelMapAux (x:rest) address = 
  case x of (Inst _)  -> makeLabelMapAux rest address+1
            (Label s) -> (s, address):makeLabelMapAux rest address
        
main :: IO()
main = do
    f <- readFile "eatertest.asm"
    let result = parseEaterASM f
    case result of Left _ -> putStrLn "ParseError"
                   Right tokens -> do
                      putStrLn $ show tokens
                      putStrLn ""
                      putStrLn $ assemble tokens




