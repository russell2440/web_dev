/* app.js
   Single-page to-do app using localStorage.
   Items stored as an array of objects:
   [{ id, text, ts }] where ts is milliseconds since epoch.
*/

(() => {
  const LS_KEY = 'simple_todo_items_v1';

  // DOM
  const todayEl = document.getElementById('today-date');
  const form = document.getElementById('todo-form');
  const input = document.getElementById('todo-input');
  const listContainer = document.getElementById('list-container');
  const emptyMsg = document.getElementById('empty-msg');

  // Utility: format date to YYYY-MM-DD (for grouping)
  function dateKey(ts) {
    const d = new Date(ts);
    const y = d.getFullYear();
    const m = String(d.getMonth() + 1).padStart(2, '0');
    const day = String(d.getDate()).padStart(2, '0');
    return `${y}-${m}-${day}`;
  }

  // Pretty date header (e.g., "Today", "Yesterday", or "Oct 31, 2025")
  function prettyDateHeader(ts) {
    const d = new Date(ts);
    const today = new Date();
    const yToday = today.getFullYear(), mToday = today.getMonth(), dayToday = today.getDate();
    const y = d.getFullYear(), m = d.getMonth(), day = d.getDate();

    const diffDays = Math.floor((new Date(yToday, mToday, dayToday) - new Date(y, m, day)) / (24*60*60*1000));

    if (diffDays === 0) return 'Today';
    if (diffDays === 1) return 'Yesterday';

    // e.g., "Nov 3, 2025"
    return d.toLocaleDateString(undefined, { month: 'short', day: 'numeric', year: 'numeric' });
  }

  // Format time e.g., "14:05"
  function timeShort(ts) {
    const d = new Date(ts);
    return d.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
  }

  // load items from localStorage
  function loadItems() {
    try {
      const raw = localStorage.getItem(LS_KEY);
      if (!raw) return [];
      const arr = JSON.parse(raw);
      if (!Array.isArray(arr)) return [];
      // defensive: ensure objects have required props
      return arr.filter(it => it && typeof it.id === 'string' && typeof it.ts === 'number' && typeof it.text === 'string');
    } catch (e) {
      console.error('Could not parse localStorage', e);
      return [];
    }
  }

  // save items to localStorage
  function saveItems(items) {
    localStorage.setItem(LS_KEY, JSON.stringify(items));
  }

  // generate simple unique id
  function uid() {
    return 'i' + Date.now().toString(36) + Math.random().toString(36).slice(2,8);
  }

  // render UI
  function render() {
    const items = loadItems();

    // newest first
    items.sort((a,b) => b.ts - a.ts);

    if (items.length === 0) {
      emptyMsg.style.display = 'block';
      listContainer.innerHTML = '';
      return;
    } else {
      emptyMsg.style.display = 'none';
    }

    // group by dateKey
    const groups = items.reduce((acc, item) => {
      const key = dateKey(item.ts);
      if (!acc[key]) acc[key] = [];
      acc[key].push(item);
      return acc;
    }, {});

    // Build DOM: ensure groups are shown newest-date-first
    const keys = Object.keys(groups).sort((a,b) => (a < b ? 1 : -1)); // newest first

    const frag = document.createDocumentFragment();

    keys.forEach(key => {
      const groupItems = groups[key];
      // create group node
      const groupDiv = document.createElement('div');
      groupDiv.className = 'group';
      // header
      const header = document.createElement('div');
      header.className = 'group-header';
      // Use the first item's ts to get a representative date for pretty display
      header.textContent = prettyDateHeader(groupItems[0].ts);
      groupDiv.appendChild(header);

      // ul
      const ul = document.createElement('ul');
      ul.className = 'group-list';
      // items in group: newest first (already sorted globally)
      groupItems.forEach(item => {
        const li = document.createElement('li');
        li.className = 'item';

        // left: text + meta time
        const left = document.createElement('div');
        left.style.display = 'flex';
        left.style.alignItems = 'center';
        left.style.gap = '10px';

        const spanText = document.createElement('span');
        spanText.className = 'item-text';
        spanText.textContent = item.text;
        left.appendChild(spanText);

        const meta = document.createElement('span');
        meta.className = 'item-meta';
        meta.textContent = timeShort(item.ts);
        left.appendChild(meta);

        li.appendChild(left);

        // right: delete button
        const del = document.createElement('button');
        del.className = 'delete-btn';
        del.setAttribute('data-id', item.id);
        del.setAttribute('aria-label', `Delete item: ${item.text}`);
        del.textContent = 'Delete';
        li.appendChild(del);

        ul.appendChild(li);
      });

      groupDiv.appendChild(ul);
      frag.appendChild(groupDiv);
    });

    listContainer.innerHTML = '';
    listContainer.appendChild(frag);
  }

  // add item
  function addItem(text) {
    const items = loadItems();
    const newItem = { id: uid(), text: text.trim(), ts: Date.now() };
    items.push(newItem);
    saveItems(items);
    render();
  }

  // delete item by id
  function deleteItemById(id) {
    let items = loadItems();
    const beforeLen = items.length;
    items = items.filter(it => it.id !== id);
    if (items.length === beforeLen) return; // nothing removed
    saveItems(items);
    render();
  }

  // update today's date in top-left
  function updateTodayDisplay() {
    const now = new Date();
    const formatted = now.toLocaleDateString(undefined, { weekday: 'short', month: 'short', day: 'numeric', year: 'numeric' });
    todayEl.textContent = formatted;
  }

  // init
  function init() {
    updateTodayDisplay();
    render();

    // Form submit (press Enter or click Add)
    form.addEventListener('submit', (ev) => {
      ev.preventDefault();
      const value = input.value;
      if (!value || !value.trim()) return;
      addItem(value);
      input.value = '';
      input.focus();
    });

    // Event delegation for deletes
    listContainer.addEventListener('click', (ev) => {
      const btn = ev.target.closest('.delete-btn');
      if (!btn) return;
      const id = btn.getAttribute('data-id');
      if (!id) return;
      deleteItemById(id);
    });

    // Optional: refresh the "Today" label at midnight
    const now = new Date();
    const msUntilMidnight = (new Date(now.getFullYear(), now.getMonth(), now.getDate() + 1) - now) + 50;
    setTimeout(() => {
      updateTodayDisplay();
      render();
      setInterval(() => {
        updateTodayDisplay();
        render();
      }, 24 * 60 * 60 * 1000);
    }, msUntilMidnight);
  }

  // Kick off
  init();
})();
