/*
 * Copyright (C) 2017 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <openssl/cipher.h>

void ENGINE_cleanup(void) {
}

void OBJ_cleanup(void) {
}

int EVP_DecryptFinal(EVP_CIPHER_CTX *ctx, uint8_t *out, int *out_len) {
	return EVP_DecryptFinal_ex(ctx, out, out_len);
}

int EVP_EncryptFinal(EVP_CIPHER_CTX *ctx, uint8_t *out, int *out_len) {
	return EVP_EncryptFinal_ex(ctx, out, out_len);
}
