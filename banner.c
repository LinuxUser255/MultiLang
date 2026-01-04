#include <stdio.h>
#include <string.h>
#include "banner.h"

#define BANNER_WIDTH 80
#define MIN_SEPARATOR_WIDTH 50

/**
 * Expected output:
 *
 *                     Multi Programming Language Codebase
 *                ══════════════════════════════════════════════════
 *                     A software development education project
 *
 */

/**
 * Prints a string centered within the banner width
 */
static void print_centered(const char *text) {
    size_t text_len = strlen(text);
    if (text_len >= BANNER_WIDTH) {
        printf("%s\n", text);
        return;
    }

    size_t total_padding = BANNER_WIDTH - text_len;
    size_t left_padding = total_padding / 2;
    size_t right_padding = total_padding - left_padding; // Handle odd widths

    printf("%*s%s%*s\n", (int)left_padding, "", text, (int)right_padding, "");
}

/**
 * Prints a horizontal separator line centered
 */
static void print_separator(size_t width) {
    if (width < MIN_SEPARATOR_WIDTH) {
        width = MIN_SEPARATOR_WIDTH;
    }

    if (width >= BANNER_WIDTH) {
        for (size_t i = 0; i < BANNER_WIDTH; i++) {
            printf("═");
        }
        printf("\n");
        return;
    }

    size_t total_padding = BANNER_WIDTH - width;
    size_t left_padding = total_padding / 2;

    printf("%*s", (int)left_padding, "");
    for (size_t i = 0; i < width; i++) {
        printf("═");
    }
    printf("\n");
}

/**
 * Calculates the maximum length among multiple strings
 */
static size_t max_length(const char *str1, const char *str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    return (len1 > len2) ? len1 : len2;
}

void print_banner(void) {
    const char *title = "Multi Programming Language Codebase";
    const char *subtitle = "A software development education project";

    // Calculate separator width based on content
    size_t content_width = max_length(title, subtitle);
    size_t separator_width = content_width + 6; // Add padding

    printf("\n");
    printf("\n");

    print_centered(title);
    print_separator(separator_width);
    print_centered(subtitle);

    printf("\n");
    printf("\n");
}
