/*
 * Rig String Functions - NOT FINALIZED, SUBJECT TO CHANGE!
 */

// Rig Flags values
#define RIG_STR_THREADSAFE ((uint16_t)(1 << 0))
#define RIG_STR_UTF8       ((uint16_t)(1 << 1))
#define RIG_STR_BINARY     ((uint16_t)(1 << 2))
#define RIG_STR_IMMUTABLE  ((uint16_t)(1 << 3))
#define RIG_STR_EXTERNAL   ((uint16_t)(1 << 4))

// Structure representing Rig strings and their associated data
struct rig_str {
	size_t maxlen; // Maximum length
	size_t len; // Actual length
	size_t pos; // Position
	uint8_t *str; // Pointer to actual string
	RIG_MRWLOCK lock; // Pointer to MRW lock (for thread-safety)
	RIG_COUNTER refcount; // Reference count
	uint16_t flags; // Flags for string behavior
};

// Macros for string literals conversion and usage
#define RIG_LIT(X)      &(const struct rig_str) { .maxlen = (sizeof(X) - 1), .len = (sizeof(X) - 1), .pos = 0, .str = (uint8_t *)(X), .lock = NULL, .refcount = NULL, .flags = RIG_STR_IMMUTABLE | RIG_STR_EXTERNAL }
#define RIG_LIT_UTF8(X) &(const struct rig_str) { .maxlen = (sizeof(X) - 1), .len = (sizeof(X) - 1), .pos = 0, .str = (uint8_t *)(X), .lock = NULL, .refcount = NULL, .flags = RIG_STR_IMMUTABLE | RIG_STR_EXTERNAL | RIG_STR_UTF8 }
#define RIG_LIT_BIN(X)  &(const struct rig_str) { .maxlen = (sizeof(X) - 1), .len = (sizeof(X) - 1), .pos = 0, .str = (uint8_t *)(X), .lock = NULL, .refcount = NULL, .flags = RIG_STR_IMMUTABLE | RIG_STR_EXTERNAL | RIG_STR_BINARY }

// Typedefs for Rig strings
typedef struct rig_str *RIG;
typedef const struct rig_str *CONST_RIG;

RIG rig_str_init(size_t maxlen, uint16_t flags);
RIG rig_str_init_from_str(size_t maxlen, uint16_t flags, const char *s);
RIG rig_str_init_from_nstr(size_t maxlen, uint16_t flags, const char *s, size_t s_len);
RIG rig_str_duplicate(CONST_RIG s, uint16_t flags); // RESETS POS TO 0 FOR NEW STR
RIG rig_str_newref(RIG s);
bool rig_str_destroy(RIG *s);
bool rig_str_resize(RIG s, size_t maxlen); // RESETS POS TO 0
bool rig_str_reset(RIG s); // RESETS POS TO 0
bool rig_str_debuginfo(CONST_RIG s);
ssize_t rig_str_maxlen(void);

const char *rig_str_getstr_ro(CONST_RIG s);
char *rig_str_getstr_rw(RIG s);
bool rig_str_returnstr(RIG s, size_t len);
bool rig_str_setpos(RIG s, ssize_t pos);
bool rig_str_setpos_ptr(RIG s, uintptr_t pos_ptr);
ssize_t rig_str_getpos(CONST_RIG s);
ssize_t rig_str_getmaxlen(CONST_RIG s);
ssize_t rig_str_getlen(CONST_RIG s);

bool rig_str_nappend(RIG dest, CONST_RIG src, size_t len);
static inline bool rig_str_append(RIG dest, CONST_RIG src) { return (rig_str_nappend(dest, src, 0)); }
static inline bool rig_str_nconcat(RIG dest, CONST_RIG src, size_t len) { return (rig_str_nappend(dest, src, len)); }
static inline bool rig_str_concat(RIG dest, CONST_RIG src) { return (rig_str_nappend(dest, src, 0)); }
bool rig_str_nprepend(RIG dest, CONST_RIG src, size_t len);
static inline bool rig_str_prepend(RIG dest, CONST_RIG src) { return (rig_str_nprepend(dest, src, 0)); }
bool rig_str_ncopy(RIG dest, CONST_RIG src, size_t len);
static inline bool rig_str_copy(RIG dest, CONST_RIG src) { return (rig_str_ncopy(dest, src, 0)); }
ssize_t rig_str_npcopy(RIG dest, CONST_RIG src, size_t len);
static inline ssize_t rig_str_pcopy(RIG dest, CONST_RIG src) { return (rig_str_npcopy(dest, src, 0)); }
ssize_t rig_str_chr(CONST_RIG s, uint32_t c);
ssize_t rig_str_rchr(CONST_RIG s, uint32_t c);
ssize_t rig_str_str(CONST_RIG haystack,CONST_RIG needle);
ssize_t rig_str_rstr(CONST_RIG haystack, CONST_RIG needle);
ssize_t rig_str_casestr(CONST_RIG haystack, CONST_RIG needle);
ssize_t rig_str_rcasestr(CONST_RIG haystack, CONST_RIG needle);
ssize_t rig_str_pbrk(CONST_RIG s, CONST_RIG accept);
ssize_t rig_str_cpbrk(CONST_RIG s, CONST_RIG reject);
ssize_t rig_str_spn(CONST_RIG s, CONST_RIG accept);
ssize_t rig_str_cspn(CONST_RIG s, CONST_RIG reject);
bool rig_str_strip(RIG s, CONST_RIG strip);
bool rig_str_lstrip(RIG s, CONST_RIG strip);
bool rig_str_rstrip(RIG s, CONST_RIG strip);
bool rig_str_nset(RIG s, uint8_t c, size_t len);
static inline bool rig_str_set(RIG s, uint8_t c) { return (rig_str_nset(s, c, 0)); }
bool rig_str_flatten(RIG s);
bool rig_str_reverse(RIG s);
bool rig_str_replace(RIG s, CONST_RIG find, CONST_RIG replace);
bool rig_str_replace_all(RIG s, CONST_RIG find, CONST_RIG replace);
RIG *rig_str_split(CONST_RIG s, uint32_t c);
bool rig_str_join(RIG s, uint32_t c, RIG *strings);
ssize_t rig_str_nlen(CONST_RIG s, size_t len);
static inline ssize_t rig_str_len(CONST_RIG s) { return (rig_str_nlen(s, 0)); }

int rig_str_ncmp(CONST_RIG s1,CONST_RIG s2, size_t len);
static inline int rig_str_cmp(CONST_RIG s1, CONST_RIG s2) { return (rig_str_ncmp(s1, s2, 0)); }
int rig_str_ncasecmp(CONST_RIG s1, CONST_RIG s2, size_t len);
static inline int rig_str_casecmp(CONST_RIG s1, CONST_RIG s2) { return (rig_str_ncasecmp(s1, s2, 0)); }
static inline bool rig_str_equal(CONST_RIG s1, CONST_RIG s2) { return (rig_str_ncmp(s1, s2, 0) == 0); }
static inline bool rig_str_caseequal(CONST_RIG s1, CONST_RIG s2) { return (rig_str_ncasecmp(s1, s2, 0) == 0); }

bool rig_str_tolower(RIG s);
bool rig_str_toupper(RIG s);

bool rig_str_isalnum(CONST_RIG s);
bool rig_str_isalpha(CONST_RIG s);
bool rig_str_isblank(CONST_RIG s);
bool rig_str_iscntrl(CONST_RIG s);
bool rig_str_isdigit(CONST_RIG s);
bool rig_str_isgraph(CONST_RIG s);
bool rig_str_islower(CONST_RIG s);
bool rig_str_isprint(CONST_RIG s);
bool rig_str_ispunct(CONST_RIG s);
bool rig_str_isspace(CONST_RIG s);
bool rig_str_isupper(CONST_RIG s);
bool rig_str_isxdigit(CONST_RIG s);

// TODO: how to support fast Unicode character access?
// TODO: hashing and serialization for strings
// TODO: "secure" mode: initialize to 0s, when doing operations, overwrite unused space, when freeing, clean up
