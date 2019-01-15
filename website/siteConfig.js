const siteConfig = {
  title: 'bs-decode',
  tagline: 'Type-safe JSON decoding for ReasonML and OCaml',
  url: 'https://mlms13.github.io',
  baseUrl: '/bs-decode/',

  projectName: 'bs-decode',
  organizationName: 'mlms13',

  // For no header links in the top nav bar -> headerLinks: [],
  headerLinks: [
    { doc: 'what-and-why', label: 'Docs' },
    { href: 'https://github.com/mlms13/bs-decode', label: 'GitHub' },
  ],

  users: [
    {
      caption: 'Home Bay',
      image: '/img/homebay.svg',
      infoLink: 'https://www.homebay.com',
      pinned: true,
    },
  ],

  /* path to images for header/footer */
  // headerIcon: 'img/docusaurus.svg',
  footerIcon: 'img/docusaurus.svg',
  favicon: 'img/favicon.png',

  /* Colors for website */
  colors: {
    primaryColor: '#2299bb',
    secondaryColor: '#1188aa',
  },

  copyright: `Copyright © ${new Date().getFullYear()} Michael Martin-Smucker`,

  highlight: {
    theme: 'default',
  },

  scripts: [],

  // On page navigation for the current documentation page.
  onPageNav: 'separate',
  cleanUrl: true,

  // Open Graph and Twitter card images.
  ogImage: null, //'img/docusaurus.png',
  twitterImage: null, //'img/docusaurus.png',

  repoUrl: 'https://github.com/mlms13/bs-decode',
};

module.exports = siteConfig;
