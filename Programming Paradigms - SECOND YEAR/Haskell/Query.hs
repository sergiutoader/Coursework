module Query where

import Data.List
import Numeric
import UserInfo
import Rating
import Movie

type Column = String
type TableSchema = [Column]
type Field = String
type Entry = [Field]

data Table = Table TableSchema [Entry]

type ColSeparator = Char
type LnSeparator = Char

{-
    Se face split la Entry-uri dupa separatorul de linii,
    apoi split pe fiecare linie dupa separatorul de coloane.
    Table-ul rezultat va avea ca TableSchema primul element
    din lista rezultata, iar lista de Entry-ul va fi coada
    listei rezultate
-}
read_table :: ColSeparator -> LnSeparator -> String -> Table
read_table colSep lineSep text = Table header entries where
    table = map (splitBy colSep) (splitBy lineSep text)
    header = head table
    entries = tail table
    splitBy :: Char -> Field -> Entry
    splitBy del = foldr op [] where
        op x [] = if x == del then [[]] else [[x]]
        op x (y:ys) = if x == del then []:(y:ys) else (x:y):ys

user_info = read_table '|' '\n' user_info_str
rating = read_table ' ' '\n' rating_str
movie = read_table '|' '\n' movie_str

-- getteri pentru Header si Entries
getHeader :: Table -> TableSchema
getHeader (Table header entries) = header

getEntries :: Table -> [Entry]
getEntries (Table header entries) = entries

{-
    Se face o concatenare intre:
        - un delimitator orizontal
        - header-ul la care s-au adaugat spatii si delimitatori verticali
        - alt delimitator orizontal
        - Entry-urile din tabel cu spatii si delimitatori
        - un alt delimitator orizontal
-} 
show_table :: Table -> String
show_table table = (getHorizontalDelimiter $ maxLen) ++
                    (addVerticalDelimiters spacedHeader) ++
                    (getHorizontalDelimiter $ maxLen) ++
                    (foldr (\a b -> (addVerticalDelimiters a)++b) []
                    spacedEntries) ++ (getHorizontalDelimiter $ maxLen) where
    --adaugare spatii la tabel
    spacedTable = addSpacesToTable table
    maxLen = getLineLength table
    spacedHeader = getHeader $ spacedTable
    spacedEntries = getEntries $ spacedTable

    -- obtine dimensiunile pentru fiecare element din tabel (se face map cu
    -- functia length pe fiecare entry)
    getLengths :: Table -> [[Int]]
    getLengths (Table header entries) = ((map length header):(map
        (map length) entries))

    -- obtine maximul pe coloana din tabel
    getMaxLengths :: [[Int]] -> [Int]
    getMaxLengths ([]:_) = []
    getMaxLengths list = (maximum $ map head list) :
        (getMaxLengths $ map tail list)

    -- functie care intoarce lungimea unei linii din afisarea tabelei
    getLineLength :: Table -> Int
    getLineLength (Table header entries) = (length header) + 1 + (foldr (+) 0
        (getMaxLengths $ (getLengths (Table header entries))))

    -- functie care intoarce un delimitator de o dimensiune data
    getHorizontalDelimiter :: Int -> String
    getHorizontalDelimiter maxLen = (replicate maxLen '-')++"\n"

    -- functie care construieste un entry al afisarii cu delimitatori verticali intre field-uri
    addVerticalDelimiters :: Entry -> String
    addVerticalDelimiters entry = (foldr (\a b -> "|"++a++b) "|" entry)++"\n"

    -- combina rezultatele functiilor de adaugare spatiu in header si entries
    addSpacesToTable :: Table -> Table
    addSpacesToTable (Table header entries) = Table (addSpacesToHeader $ Table
        header entries) (addSpacesToEntries $ Table header entries) where
        -- adauga n spatii la finalul lui Field
        addSpacesToField :: Field -> Int -> Field
        addSpacesToField str n = str ++ (replicate n ' ')
        -- adauga spatii la header
        addSpacesToHeader :: Table -> TableSchema
        addSpacesToHeader (Table header entries) = zipWith (\x y ->
            addSpacesToField x (y - (length x))) header maxLengths where
            maxLengths = (getMaxLengths $ getLengths $ Table header entries)
        -- adauga spatii la entries
        addSpacesToEntries :: Table -> [Entry]
        addSpacesToEntries (Table header entries) = map (zipWith (\x y ->
            addSpacesToField y (x - (length y))) maxLengths) entries where
            maxLengths = (getMaxLengths $ getLengths $ Table header entries)

instance Show Table where
    show = show_table



-- Functii folosite pentru concatenarea/selectarea coloanelor din tabel

-- obtine indexul unei coloane din tabel in functie de numele din header
getColumnIndex :: TableSchema -> Column -> Int
getColumnIndex header column = findColumn header column 1 where
    findColumn :: TableSchema -> Column -> Int -> Int
    findColumn [] column acc = -1
    findColumn (x:xs) column acc = if column == x then acc else
        findColumn xs column (acc + 1)

-- extrage o coloana din tabel in functie de indicele sau
getColumn :: Int -> Table -> Table
getColumn nr (Table header entries) = Table ([(head . drop (nr - 1) ) header])
    (map (\x-> ((head (drop (nr - 1) x)):[])) entries)

-- Functie care alipeste 2 tabele folosind concatenare simpla in
-- cazul header-ului si zipWith cu concatenare pentru Entry-uri
mergeTables :: Table -> Table -> Table
mergeTables (Table header1 entries1) (Table header2 entries2) =
    (Table (header2 ++ header1) (zipWith (++) entries2 entries1))

-- intoarce o lista cu toti indicii coloanelor din tabel
getAllColumnIndexes :: [String] -> Table -> [Int]
getAllColumnIndexes list (Table header entries) =
    map (\col -> getColumnIndex header col) list

-- construieste tabeulul prin alipirea tuturor coloanelor la o coloana vida folosind foldr
-- se foloseste functia replicate pentru a genera un vector de entry-uri cu n linii goale
mergeAllColumns :: Table -> [Int] -> Table
mergeAllColumns table indexes = foldr (\x y -> mergeTables y (getColumn x table))
    (Table [] (replicate (length $ getEntries table) []) ) indexes



-- Functii folosite pentru Select Limited

-- similara functiei getColumn, doar ca limiteaza numarul de intrari luate din tabel cu limit
getColumnLimited :: Int -> Int -> Table -> Table
getColumnLimited nr limit (Table header entries) =
    Table ([(head . drop (nr - 1) ) header]) (take limit $ map
        (\x-> ((head (drop (nr - 1) x)):[])) entries)

-- similara functiei getAllColumns, doar ca foloseste functia getColumnLimited 
mergeAllColumnsLimited :: Table -> Int -> [Int] -> Table
mergeAllColumnsLimited table limit indexes = foldr (\x y -> mergeTables y
    (getColumnLimited x limit table)) (Table []
    (replicate (length $ getEntries table) []) ) indexes



-- Functii folosite pentru implementarea filtrelor

-- functia de conditie pentru filterIn - verifica daca campul curent
-- din tabela face match cu vreuna din valorile date
valueMatch :: Int -> [Field] -> (Entry -> Bool)
valueMatch index [] entry = False
valueMatch index (x:xs) entry = if (entry !! (index - 1) == x)
    then True else valueMatch index xs entry

-- functia de conditie pentru filterLowerThan - verifica daca campul
-- curent din tabel este mai mic decat valoarea data
valueLower :: Int -> Int -> (Entry -> Bool)
valueLower index value entry = (read $ entry!!(index - 1)) < value

-- functia de conditie pentru filterEqualTo - verifica daca campul
-- curent din tabel este egal cu valoarea data
valueEqual :: Int -> Field -> (Entry -> Bool)
valueEqual index field entry = (entry!!(index - 1)) == field

-- functii de conditie "negate" ale celor 3 de mai sus - folosite in implementarea lui Not
valueNoMatch :: Int -> [Field] -> (Entry -> Bool)
valueNoMatch index [] entry = True
valueNoMatch index (x:xs) entry = if (entry !! (index - 1) == x)
    then False else valueNoMatch index xs entry

valueHigher :: Int -> Int -> (Entry -> Bool)
valueHigher index value entry = (read $ entry!!(index - 1)) >= value

valueNotEqual :: Int -> Field -> (Entry -> Bool)
valueNotEqual index field entry = (entry!!(index - 1)) /= field

-- functii care filtreaza entry-urile din tabel 
filterLowerThan :: Table -> Int -> Int -> Table
filterLowerThan table column value = Table (getHeader table) (filter
    (valueLower column value) (getEntries table) )

filterEqualTo :: Table -> Int -> Field -> Table
filterEqualTo table column field = Table (getHeader table) (filter
    (valueEqual column field) (getEntries table) )

filterIn :: Table -> Int -> [Field] -> Table
filterIn table index values = Table (getHeader table) (filter
    (valueMatch index values) (getEntries table) )

-- functii folosite pentru filterul not - filtreaza de asemenea entry-urile din tabel
filterHigherThan :: Table -> Int -> Int -> Table
filterHigherThan table column value = Table (getHeader table) (filter
    (valueHigher column value) (getEntries table) )

filterNotEqualTo :: Table -> Int -> Field -> Table
filterNotEqualTo table column field = Table (getHeader table) (filter
    (valueNotEqual column field ) (getEntries table) )

filterNotIn :: Table -> Int -> [Field] -> Table
filterNotIn table index values = Table (getHeader table) (filter
    (valueNoMatch index values) (getEntries table) )

data FilterCondition = Lt Field Integer | Eq Field String
    | In Field [String] | Not FilterCondition



-- Functii folosite pentru operatia de reuniune a 2 tabele (:||) 

-- functie care realizeaza transpusa tabelei
transposeEntries :: [Entry] -> [Entry]
transposeEntries ([] : _) = []
transposeEntries mat = (map head mat) : (transpose (map tail mat))

-- functie care face horizontal join intre continutul a 2 tabele
mergeEntries :: [Entry] -> [Entry] -> [Entry]
mergeEntries entries1 entries2 = zipWith (++) entries1 entries2

-- functie care realizeaza operatia :||
-- se face merge intre transpusa primului tabel. transpusa celui
-- de-al doilea tabel, rezultatul fiind apoi transpus
reunion :: Table -> Table -> Table
reunion (Table header1 entries1) (Table header2 entries2) = Table header1
    (transpose (mergeEntries (transposeEntries
    entries1) (transposeEntries entries2)))



data Query = Filter FilterCondition Query |  
             Select [String] Query |
             SelectLimit [String] Integer Query |
             Cosine Query |
             Query :|| Query |
             Atom Table

-- Implementarea functiei eval

eval :: Query -> Table
eval (Atom table) = table
-- Select face merge intre coloanele selectate, facandu-se o evaluare recursiv pe parametrul dat
-- SelectLimit impune limita de a lua primele <limit> linii din aceste coloane
eval (Select list x) = mergeAllColumns (eval x) $ getAllColumnIndexes list (eval x)
eval (SelectLimit list limit x) = mergeAllColumnsLimited (eval x)
    (fromIntegral limit) $ getAllColumnIndexes list (eval x)

-- Eval pentru filtre se implementeaza prin aplicarea functiilor de filter
-- pentru coloana data cu limitarile corespunzatoare fiecarui tip de filter
eval (Filter (Lt field value) (Atom table)) = filterLowerThan table
    (getColumnIndex (getHeader table) field) (fromIntegral value)
eval (Filter (Eq column value) (Atom table)) = filterEqualTo table
    (getColumnIndex (getHeader table) column) value
eval (Filter (In column values) (Atom table)) = filterIn table
    (getColumnIndex (getHeader table) column) values
-- Not va folosi celelalte functii de filtrare care produc rezultatul invers
eval (Filter (Not (Lt field value) ) (Atom table) ) = filterHigherThan table
    (getColumnIndex (getHeader table) field) (fromIntegral value)
eval (Filter (Not (Eq column value) ) (Atom table) ) = filterNotEqualTo table
    (getColumnIndex (getHeader table) column) value
eval (Filter (Not (In column values) ) (Atom table) ) = filterNotIn table
    (getColumnIndex (getHeader table) column) values
eval (Filter x (Select list y)) = eval $ (Filter x) $
    (Atom (eval (Select list y)))
eval (Filter x y) =  eval $ (Filter x) $ Atom (eval y)
-- Pentru operatia :|| se foloseste functia reunion,
-- evaluandu-se recursiv query-urile primite ca parametru
eval (x :|| y) = reunion (eval x) (eval y)
eval (Cosine x) = create_cosine (eval x)

-- Functiile same_zone, male_within_age, mixed

-- se filtreaza dupa zona, apoi se elimina intrarea cu acelasi id ca
-- cel primit ca parametru, apoi se selecteaza coloanele cerute
same_zone :: String -> Query
same_zone user_id = Atom (eval $ Select ["user_id", "occupation"] $ Filter (Not
    (Eq "user_id" user_id)) $ Filter (Eq "zone" (getZone
    user_id (getEntries user_info))) $ Atom user_info) where
    -- functie care intoarce locatia userului cu ID-ul dat 
    getZone :: String -> [Entry] -> Field
    getZone value (x:xs) = if (head x) == value then (last x) else getZone value xs

-- se filtreaza dupa sex, apoi dupa varsta, selectandu-se apoi coloanele cerute
male_within_age :: Integer -> Integer -> Query
male_within_age sbound bbound = Atom ( eval $ Select ["occupation", "zone"] $
    Filter (Not (Lt "age" sbound)) $ Filter (Not (Eq "age" (show sbound)))
    $ Filter (Lt "age" bbound) $ Filter (Eq "sex" "M") $ Atom user_info )

-- selecteaza liniile care au match pe una din ocupatii, apoi pe una din zone,
-- apoi se face selectia dupa varsta si se alege coloana user_id
mixed :: [String] -> [String] -> Int -> Query
mixed zones occupations age = Atom (eval $ Select ["user_id"] $ Filter
    (Lt "age" (toInteger age)) $ Filter (In "zone" zones) $ Filter
    (In "occupation" occupations) $ Atom user_info)

-- bonus

-- obtin id-ul maxim

get_max_id :: [Entry] -> Int
get_max_id entries = aux entries 0 where
    aux [] acc = acc
    aux (x:xs) acc = if (read (head x)) > acc then aux xs (read
        (head x)) else aux xs acc  

-- functie care genereaza o lista cu toate notele date de utilizatori pentru filme
create_pair_list :: [Entry] -> [[(Field, Field)]]
create_pair_list entries = aux entries (get_empty_list $ get_max_id entries) where
    aux :: [Entry] -> [[(Field, Field)]] -> [[(Field, Field)]]
    aux [] acc = acc
    aux (x:xs) acc = aux xs (addAt acc (get_pair x) (read (head x)))
    -- functie care adauga un element intr-o lista de liste pe o pozitie data
    addAt :: [[a]] -> a -> Int -> [[a]]
    addAt (x:xs) pair 1 = (pair:x):xs
    addAt (x:xs) pair n = x :(addAt xs pair (n-1))  
    -- obtine  o pereche (film, nota) de la un entry
    get_pair :: Entry -> (Field, Field)
    get_pair entry = (entry !! 1, entry !! 2)
    -- genereaza o lista de liste vide
    get_empty_list :: Int -> [[(Field, Field)]]
    get_empty_list count = replicate count []


-- sorteaza listele dupa id-ul filmelor
sort_pair_list ::  [[(Field, Field)]] -> [[(Field, Field)]]
sort_pair_list list = map (sortBy sort_movie) list where
    sort_movie x y = compare (fst x) (fst y)

-- calculeaza produsul xi * yi, comparand mereu primele elemente din cele 2 liste
-- daca elementele sunt egale, se adauga la acumulator produsul lor
-- daca unul este mai mare, se trece la urmatorul in cealalta lista
get_product :: [(Field, Field)] -> [(Field, Field)] -> Float
get_product x y = aux x y 0 where
    aux [] _ acc = acc
    aux _ [] acc = acc
    aux (x:xs) (y:ys) acc = if (fst x) > (fst y) then (aux (x:xs) ys acc)
        else if (fst x) < (fst y) then (aux xs (y:ys) acc) else
            aux xs ys (acc + (read $ snd x)*(read $ snd y))

-- asociaza unei liste radical din suma patratelor valorilor
-- (pentru a nu se calcula de fiecare data)
pair_denominator :: [(Field, Field)] -> ([(Field, Field)], Float)
pair_denominator list = (list, (sqrt (foldr (\x y -> ((read
    (snd x)) ^ 2) + y) 0 list) ))


-- functie care intoace toate perechile de useri posibile
get_paired_users :: Int -> [(Int, Int)]
get_paired_users max_id = [(x, y) | x <- [1 ..max_id],
    y <-[1..max_id], (show x) < (show y)]


-- calculeaza scorul pentru 2 useri (avand liste de elemente de tip (film - nota))
get_score :: ([(Field, Field)], Float) -> ([(Field, Field)], Float) -> Float
get_score a b = (getNumerator a b) / (getDenominator a b) where
    -- calculeaza numitorul ca suma din xi*yi
    getDenominator a b = (snd a) * (snd b)
    getNumerator a b = get_product (fst a) (fst b)

-- functie care primeste 2 user-id si creeaza un entry din tabel
create_entry :: (Int, Int) -> ([(Field, Field)], Float)
    -> ([(Field, Field)], Float) -> Entry
create_entry (u1, u2) ratings1 ratings2 = (show u1) : (show u2) :
    (showFFloat (Just 4) (get_score ratings1 ratings2) ""):[]

-- intoarce o lista de perechi de la o pozitie data
get_list_at_index :: Int -> [([(Field, Field)], Float)]
    -> ([(Field, Field)], Float)
get_list_at_index user_id list = list !! (user_id - 1)

-- construiesc tabelul cu user1 user2 scor
create_entries :: [([(Field, Field)], Float)] -> [(Int, Int)] -> [Entry]
create_entries pair_list [] = []
create_entries pair_list (x:xs) = (create_entry x (get_list_at_index (fst x)
    pair_list) (get_list_at_index (snd x) pair_list)
    ) : (create_entries pair_list xs)

-- functia principala care creeaza tabelul
create_cosine :: Table -> Table
create_cosine (Table header entries) = (Table ["user_id1", "user_id2", "sim"]
    (filter (\x -> (x !! 2) /= "NaN") (sortBy sortGT (create_entries
        (map pair_denominator $ sort_pair_list $ create_pair_list entries)
        (get_paired_users (get_max_id entries))))))

-- functie auxiliara pentru sortare
sortGT x y
  | (head x) > (head y) = GT
  | (head x) < (head y) = LT
  | (head x) == (head y) = compare (head $ tail x) (head $ tail y)

