# syntax=docker/dockerfile:1
# BusTub dev environment: Ubuntu 24.04 + clang-15, matching the grading environment.

FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
ARG CLANG_VERSION=15
ARG USERNAME=node
ARG USER_UID=1000
ARG USER_GID=1000

ENV LANG=C.UTF-8 \
    LC_ALL=C.UTF-8 \
    CC=clang-15 \
    CXX=clang++-15 \
    CCACHE_DIR=/home/${USERNAME}/.ccache \
    CCACHE_MAXSIZE=5G

COPY build_support/packages.sh /tmp/packages.sh

# Reuse the course-provided package list, then add tools it does not install.
RUN bash /tmp/packages.sh -y \
    && apt-get install -y --no-install-recommends \
        bash-completion \
        ca-certificates \
        ccache \
        curl \
        gdb \
        less \
        libclang-rt-${CLANG_VERSION}-dev \
        lldb \
        ninja-build \
        python3 \
        sudo \
        vim \
        wget \
    && rm -rf /var/lib/apt/lists/* /tmp/packages.sh

RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-${CLANG_VERSION} 100 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-${CLANG_VERSION} 100 \
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-${CLANG_VERSION} 100 \
    && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-${CLANG_VERSION} 100

# Ubuntu ships an "ubuntu" user with UID/GID 1000; replace it with the dev user.
RUN set -eux; \
    if getent passwd "${USERNAME}" >/dev/null 2>&1; then userdel -r "${USERNAME}" 2>/dev/null || true; fi; \
    if getent group "${USERNAME}" >/dev/null 2>&1; then groupdel "${USERNAME}" 2>/dev/null || true; fi; \
    if getent group "${USER_GID}" >/dev/null 2>&1; then \
        groupmod -n "${USERNAME}" "$(getent group "${USER_GID}" | cut -d: -f1)"; \
    else \
        groupadd --gid "${USER_GID}" "${USERNAME}"; \
    fi; \
    if getent passwd "${USER_UID}" >/dev/null 2>&1; then \
        userdel -r "$(getent passwd "${USER_UID}" | cut -d: -f1)" 2>/dev/null || true; \
    fi; \
    useradd --uid "${USER_UID}" --gid "${USER_GID}" --create-home --shell /bin/bash "${USERNAME}"; \
    echo "${USERNAME} ALL=(ALL) NOPASSWD:ALL" > "/etc/sudoers.d/${USERNAME}"; \
    chmod 0440 "/etc/sudoers.d/${USERNAME}"; \
    mkdir -p "${CCACHE_DIR}"; \
    chown -R "${USER_UID}:${USER_GID}" "/home/${USERNAME}"

# Some LLDB versions only ship a versioned lldb-dap binary.
RUN set -eux; \
    if ! command -v lldb-dap >/dev/null 2>&1; then \
        lldb_dap="$(find /usr/bin /usr/lib/llvm-* -maxdepth 3 -type f -name 'lldb-dap*' 2>/dev/null | head -n 1 || true)"; \
        if [ -n "${lldb_dap}" ]; then ln -s "${lldb_dap}" /usr/local/bin/lldb-dap; fi; \
    fi

USER ${USERNAME}
WORKDIR /home/${USERNAME}
CMD ["sleep", "infinity"]
