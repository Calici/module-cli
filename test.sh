echo "test-module\n0.0.0" | pharmaco init && \
echo "test-1" | pharmaco test add && \
pharmaco test list && \
pharmaco test refresh && \
pharmaco test reset all && \
pharmaco test fix && \
pharmaco test all && \
pharmaco container name && \
pharmaco container validate && \
pharmaco refresh lock && \
echo "test-module" | pharmaco rename && \
pharmaco relock