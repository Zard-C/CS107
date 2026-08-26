const filter = document.querySelector("#lecture-filter");
const cards = Array.from(document.querySelectorAll(".lecture-card"));
const emptyState = document.querySelector("#empty-state");

if (window.location.protocol === "file:") {
  for (const link of document.querySelectorAll('a[href^="Notes/"][href$=".html"]')) {
    const href = link.getAttribute("href");
    link.setAttribute("href", href.replace(/\.html$/, ".md"));
  }
}

if (filter && cards.length && emptyState) {
  filter.addEventListener("input", () => {
    const query = filter.value.trim().toLowerCase();
    let visible = 0;

    for (const card of cards) {
      const haystack = `${card.textContent} ${card.dataset.title || ""}`.toLowerCase();
      const match = !query || haystack.includes(query);
      card.hidden = !match;
      if (match) visible += 1;
    }

    emptyState.hidden = visible !== 0;
  });
}
